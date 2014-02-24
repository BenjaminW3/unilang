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
#ifdef USE_MCJIT
#include <llvm/ExecutionEngine/MCJIT.h>	// needed to do MCJIT
#else
#include <llvm/ExecutionEngine/JIT.h>
#endif
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
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		int64_t execute_module( llvm::Module const & module )
		{
			llvm::InitializeNativeTarget();
#ifdef USE_MCJIT
			llvm::InitializeNativeTargetAsmPrinter();
			llvm::InitializeNativeTargetAsmParser();
#endif

			// llvm::EngineBuilder takes ownership of the module so we have to copy it first.
			std::unique_ptr<llvm::Module> moduleCopy(llvm::CloneModule(&module));

			std::string ErrStr;
			std::unique_ptr<llvm::ExecutionEngine> ee(
				llvm::EngineBuilder(moduleCopy.release())
					.setEngineKind(llvm::EngineKind::JIT)
#ifdef USE_MCJIT
					.setUseMCJIT(true)
#endif
					.setOptLevel(llvm::CodeGenOpt::Default)
					.setErrorStr(&ErrStr)
					.create());
			if(!ee)
			{
				throw std::runtime_error("llvm::EngineBuilder::create failed with error: '" + ErrStr + "'.");
			}

			//ee->DisableLazyCompilation();
#ifdef USE_MCJIT
			ee->finalizeObject();							// Make it ready for execution.
#endif
			ee->runStaticConstructorsDestructors(false);	// Runs the constructors.

			// Get the entrypoint mangled name.
			ast::function_declaration funcDecl;
			// TODO: Should the entrypoint function really be unpure? Purity is always better...
			funcDecl._bHasUnpureQualifier = true;
			funcDecl._idfName.m_sName = "entrypoint";
			ast::variable_declaration var;
			var._type._idfName = ast::identifier("i64");
			funcDecl._vReturnDeclarations.emplace_back(var);
			
			// Get the entrypoint function.
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

			// We could use runFunctionAsMain but this forces 32 bit return value.
			// i32RetVal = ee->runFunctionAsMain(pEntrypointFunc, {}, nullptr);

			// Get the function pointer.
			typedef int64_t (*FuncPtr)();
			FuncPtr const fptr(reinterpret_cast<FuncPtr>(ee->getPointerToFunction(pEntrypointFunc)));

			// Call the entrypoint function.
			int64_t const i64RetVal (fptr());
			
			// Clean-up
			ee->runStaticConstructorsDestructors(true);	// Runs the destructors.

			return i64RetVal;
		}
	}
}