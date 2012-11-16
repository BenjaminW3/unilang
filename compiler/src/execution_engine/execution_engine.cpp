#include "execution_engine.hpp"

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

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"	// needed to ling JIT
#include "llvm/Support/TargetSelect.h"	// InitializeNativeTarget
//#include "llvm/Analysis/Verifier.h"
//#include "llvm/Target/TargetData.h"
//#include "llvm/Transforms/Scalar.h"
#include "llvm/Module.h"

// Bitcode
#include "llvm/Support/MemoryBuffer.h"	// MemoryBuffer
#include "llvm/Support/system_error.h"
#include "llvm/Bitcode/ReaderWriter.h"	// ParseBitcodeFile
#include "llvm/LLVMContext.h"			// getGlobalContext

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <iostream>

namespace unilang
{ 
	namespace execution_engine
	{
		int execute_module( std::shared_ptr<llvm::Module> module )
		{
			llvm::InitializeNativeTarget();

			std::string ErrStr;
			std::unique_ptr<llvm::ExecutionEngine> ee(llvm::EngineBuilder(module.get()).setErrorStr(&ErrStr).create());
			if(!ee)
			{
				throw std::runtime_error(ErrStr);
			}
			llvm::Function* func = ee->FindFunctionNamed("entrypoint");
			if(!func)
			{
				throw std::runtime_error("'entrypoint' not found!");
			}

			typedef unsigned int (*FuncPtr)();
			FuncPtr fptr = reinterpret_cast<FuncPtr>(ee->getPointerToFunction(func));

			int ret_val = fptr();

			//ee->freeMachineCodeForFunction(vm_get_current_closure);
			ee->runStaticConstructorsDestructors(true);
			if(!ee->removeModule(module.get()))
			{
				throw std::runtime_error("Unable to remove module from ExecutionEngine!");
			}

			return ret_val;
		}

		int execute_bitcode_file( std::string const & sBitCodeFile )
		{
			// TODO: check file validity

			//std::unique_ptr<llvm::MemoryBuffer> buffer;
			llvm::OwningPtr<llvm::MemoryBuffer> buffer;
			llvm::error_code error = llvm::MemoryBuffer::getFile(sBitCodeFile.c_str(), buffer);
			if(!buffer)
			{
				throw std::runtime_error("Unable to getFile '" + sBitCodeFile + "Error: " + error.message());
			}

			std::string sError;
			std::shared_ptr<llvm::Module> module(llvm::ParseBitcodeFile(buffer.get(), llvm::getGlobalContext(), &sError));
			if(!module)
			{
				throw std::runtime_error("Unable to ParseBitcodeFile '" + sBitCodeFile + "' with error" + sError);
			}

			return execute_module(module);
		}
	}
}