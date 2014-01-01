#include <unilang/compiler/execution_engine/execution_engine.hpp>

#include <unilang/compiler/ast/function_ast.hpp>

#include <unilang/compiler/code_generator/types.hpp>

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
#include <llvm/IR/Module.h>
#include <llvm/Transforms/Utils/Cloning.h>	// llvm::CloneModule

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
		int64_t execute_module( llvm::Module const & module )
		{
			llvm::InitializeNativeTarget();

			// llvm::EngineBuilder takes ownership of the module so we have to copy it first.
			std::unique_ptr<llvm::Module> moduleCopy(llvm::CloneModule(&module));

			std::string ErrStr;
			std::unique_ptr<llvm::ExecutionEngine> ee(llvm::EngineBuilder(moduleCopy.release()).setEngineKind(llvm::EngineKind::JIT).setOptLevel(llvm::CodeGenOpt::Default).setErrorStr(&ErrStr).create());
			if(!ee)
			{
				throw std::runtime_error(ErrStr);
			}

			ee->DisableLazyCompilation();
			ee->runStaticConstructorsDestructors(false);	// Runs the constructors.

			ast::function_declaration funcDecl;
			// TODO: Should the entrypoint function really be unpure? Purity is always better...
			funcDecl._bHasUnpureQualifier = true;
			funcDecl._idfName._name = "entrypoint";
			funcDecl._vReturnTypes.emplace_back(ast::identifier("i64"));
			
			std::string const entrypointName(funcDecl.build_mangled_name());
			llvm::Function * const pEntrypointFunc (ee->FindFunctionNamed(entrypointName.c_str()));
			if(pEntrypointFunc==nullptr)
			{
				throw std::runtime_error("Entrypoint '"+entrypointName+"' not found!");
			}

			// The return type of the entrypoint is not checked by name mangling, so check it explicitly.
			llvm::Type const * const pReturnType(pEntrypointFunc->getReturnType());
			if(pReturnType==nullptr)
			{
				throw std::runtime_error("Return type of entrypoint '"+entrypointName+"' is invalid!");
			}
			auto const sRequiredReturnTypeName(code_generator::cTypeToUnilangTypeName<int64_t>());
			auto const sReturnTypeName(code_generator::llvmTypeToUnilangTypeName(pReturnType));
			if(sReturnTypeName!=sRequiredReturnTypeName)
			{
				throw std::runtime_error("Return type of entrypoint '"+entrypointName+"' is '"+sReturnTypeName+"' instead of '"+sRequiredReturnTypeName+"' !");
			}

			// Get the function pointer.
			typedef int64_t (*FuncPtr)();
			FuncPtr const fptr(reinterpret_cast<FuncPtr>(ee->getPointerToFunction(pEntrypointFunc)));

			int64_t const i64RetVal (fptr());
			
			ee->runStaticConstructorsDestructors(true);	// Runs the destructors.

			return i64RetVal;
		}
	}
}