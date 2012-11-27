#include "execution_engine.hpp"

#include "../ast/function_ast.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// assignment operator could not be generated
#pragma warning(disable: 4800)		// 'unsigned int' : forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>	// needed to do JIT
#include <llvm/Support/TargetSelect.h>	// InitializeNativeTarget
//#include <llvm/Analysis/Verifier.h>
//#include <llvm/Target/TargetData.h>
//#include <llvm/Transforms/Scalar.h>
#include <llvm/Module.h>

// Bitcode
#include <llvm/Support/MemoryBuffer.h>	// MemoryBuffer
#include <llvm/Support/system_error.h>
#include <llvm/Bitcode/ReaderWriter.h>	// ParseBitcodeFile
#include <llvm/LLVMContext.h>			// getGlobalContext

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <iostream>

namespace unilang
{ 
	namespace execution_engine
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		int64_t execute_module( std::shared_ptr<llvm::Module> module )
		{
			llvm::InitializeNativeTarget();

			std::string ErrStr;
			std::unique_ptr<llvm::ExecutionEngine> ee(llvm::EngineBuilder(module.get()).setEngineKind(llvm::EngineKind::JIT).setOptLevel(llvm::CodeGenOpt::Default).setErrorStr(&ErrStr).create());
			if(!ee)
			{
				throw std::runtime_error(ErrStr);
			}

			ee->DisableLazyCompilation();
			ee->runStaticConstructorsDestructors(false);

			ast::function_declaration funcDecl;
			funcDecl._identifier._name = "entrypoint";
			funcDecl._return_types.emplace_back(ast::identifier("i64"));
			
			std::string const entrypointName(funcDecl.build_mangled_name());
			llvm::Function* func = ee->FindFunctionNamed(entrypointName.c_str());
			if(func->isNullValue())
			{
				throw std::runtime_error("'"+entrypointName+"' not found!");
			}

			typedef int64_t (*FuncPtr)();
			FuncPtr fptr (reinterpret_cast<FuncPtr>(ee->getPointerToFunction(func)));

			int64_t ret_val = fptr();
			
			ee->runStaticConstructorsDestructors(true);
			ee->clearGlobalMappingsFromModule(module.get());
			//ee->freeMachineCodeForFunction(vm_get_current_closure);
			if(!ee->removeModule(module.get()))
			{
				throw std::runtime_error("Unable to remove module from ExecutionEngine!");
			}

			return ret_val;
		}
		
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		int64_t execute_bitcode_file( std::string const & sBitCodeFilePath )
		{
			// TODO: check file validity

			//std::unique_ptr<llvm::MemoryBuffer> buffer;
			llvm::OwningPtr<llvm::MemoryBuffer> buffer;
			llvm::error_code const error (llvm::MemoryBuffer::getFile(sBitCodeFilePath.c_str(), buffer));
			if(!buffer)
			{
				throw std::runtime_error("Unable to getFile '" + sBitCodeFilePath + "Error: " + error.message());
			}

			std::string sError;
			std::shared_ptr<llvm::Module> module(llvm::ParseBitcodeFile(buffer.get(), llvm::getGlobalContext(), &sError));
			if(!module)
			{
				throw std::runtime_error("Unable to ParseBitcodeFile '" + sBitCodeFilePath + "' with error" + sError);
			}

			return execute_module(module);
		}
	}
}