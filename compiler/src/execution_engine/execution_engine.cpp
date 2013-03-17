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
#include <llvm/Module.h>

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
		int64_t execute_module( llvm::Module & module )
		{
			llvm::InitializeNativeTarget();

			std::string ErrStr;
			std::unique_ptr<llvm::ExecutionEngine> ee(llvm::EngineBuilder(&module).setEngineKind(llvm::EngineKind::JIT).setOptLevel(llvm::CodeGenOpt::Default).setErrorStr(&ErrStr).create());
			if(!ee)
			{
				throw std::runtime_error(ErrStr);
			}

			ee->DisableLazyCompilation();
			ee->runStaticConstructorsDestructors(false);

			ast::function_declaration funcDecl;
			funcDecl._idfName._name = "entrypoint";
			funcDecl._vReturnTypes.emplace_back(ast::identifier("i64"));
			
			std::string const entrypointName(funcDecl.build_mangled_name());
			llvm::Function * const func (ee->FindFunctionNamed(entrypointName.c_str()));
			if(ee==nullptr || func->isNullValue())
			{
				throw std::runtime_error("'"+entrypointName+"' not found!");
			}

			typedef int64_t (*FuncPtr)();
			FuncPtr fptr (reinterpret_cast<FuncPtr>(ee->getPointerToFunction(func)));

			int64_t ret_val (fptr());
			
			ee->runStaticConstructorsDestructors(true);
			ee->clearGlobalMappingsFromModule(&module);
			//ee->freeMachineCodeForFunction(vm_get_current_closure);
			if(!ee->removeModule(&module))
			{
				throw std::runtime_error("Unable to remove module from ExecutionEngine!");
			}

			return ret_val;
		}
	}
}