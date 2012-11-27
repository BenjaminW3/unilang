#include "../code_generator.hpp"

#include "../../log/log.hpp"

#include <llvm/Analysis/Verifier.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/IRBuilder.h>
#include <llvm/Function.h>
#include <llvm/Type.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include "../types.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * code_generator::operator()(ast::function_definition const & x)
		{
			LOG_SCOPE_DEBUG;

			vLocalSymbolTable.clear();

			// purity test
			/*if ((!x.decl._bHasUnpureQualifier) && !x._body.IsPure())
			{
				return ErrorFunction("Pure function '"+x.decl._identifier._name+"' contains unpure expressions!");
			}*/
  
			// build function declaration
			ast::function_declaration decl;
			decl._identifier = x._identifier;
			decl._bHasUnpureQualifier = x._bHasUnpureQualifier;
			for(ast::variable_declaration const & x : x._parameter_declarations)
			{
				decl._parameter_types.push_back(x._type);
			}
			for(ast::variable_definition const & x : x._return_value_definitions)
			{
				decl._return_types.push_back(x._declaration._type);
			}
			llvm::Function *TheFunction = (*this)(decl);
			if (TheFunction == 0)
			{
				return ErrorFunction("Unable to build function declaration for function definition: '"+x._identifier._name+"'.");
			}

			// Create a new basic block to start insertion into.
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(getContext(), "entry", TheFunction);
			getBuilder()->SetInsertPoint(BB);

			// add the parameters
			llvm::Function::arg_iterator itArgs = TheFunction->arg_begin();
			for (size_t Idx = 0, e = x._parameter_declarations.size(); Idx != e; ++Idx, ++itArgs)
			{
				// TODO: Think about order? l->r, r->l
				llvm::AllocaInst * pDeclAlloc = (*static_cast<allocation_code_generator*>(this))(x._parameter_declarations[Idx]);
				if (!pDeclAlloc)
				{
					const auto sVarName = x._parameter_declarations[Idx]._optionalIdentifier.is_initialized() ? " '"+x._parameter_declarations[Idx]._optionalIdentifier.get()._name+"'" : "";
					return ErrorFunction("Unable to create parameter '"+ sVarName+"' for function '"+x._identifier._name+"'");
				}

				// store the parameters in the parameter variables
				getBuilder()->CreateStore(itArgs, pDeclAlloc);
			}

			// add the return values
			std::vector<llvm::AllocaInst *> vpRetAllocas;
			for (unsigned int Idx = 0; Idx != x._return_value_definitions.size(); ++Idx)
			{
				vpRetAllocas.push_back((*static_cast<allocation_code_generator*>(this))(x._return_value_definitions[Idx]));
				if(!vpRetAllocas.back())
				{
					const auto sVarName = x._return_value_definitions[Idx]._declaration._optionalIdentifier.is_initialized() ? " '"+x._return_value_definitions[Idx]._declaration._optionalIdentifier.get()._name+"'" : "";
					return ErrorFunction("Unable to create return value '"+ sVarName+"' from function '"+x._identifier._name+"'");
				}
			}

			// add body
			if(!(*static_cast<statement_code_generator*>(this))(x._body))
			{
				return ErrorFunction("Unable to create body for function '"+x._identifier._name+"'.");
			}

			// return value(s)
			if(vpRetAllocas.empty())
			{
				getBuilder()->CreateRetVoid();
			}
			else if(vpRetAllocas.size() == 1)
			{
				llvm::Value *pRetVal = getBuilder()->CreateLoad(vpRetAllocas[0], "loadret");
				if(!pRetVal)
				{
					return ErrorFunction("Unable to create load return from function '"+x._identifier._name+"'.");
				}
				if(pRetVal->getType() != TheFunction->getReturnType())
				{
					return ErrorFunction("Return type mismatch! Trying to return '"+getLLVMTypeName(pRetVal->getType())+"' from a function '"+x._identifier._name+" 'returning '"+getLLVMTypeName(TheFunction->getReturnType())+"'.");
				}
				/*if(!TheFunction->getReturnType()->isPointerTy())
				{
					if(vpRetValues[0]->isDereferenceablePointer())
					{
						//llvm::GetElementPtrInst* gep = new llvm::GetElementPtrInst(retValues[0],);
						//const llvm::PointerType * ptr_type = dynamic_cast<const llvm::PointerType*>((retValues[0]->getType()));
						//ptr_type->
					}
				}*/
				getBuilder()->CreateRet(pRetVal);
			}
			else
			{
				if(TheFunction->getReturnType()->isAggregateType())
				{
					// FIXME: handle multi-return value functions in result expressions.
					// FIXME: not alloca, instead values
					//getBuilder()->CreateAggregateRet(vpRetAllocas.data(), (unsigned int)x._return_value_definitions.size());
					return ErrorFunction("Unable to return multiple return values from aggregate return type function '"+x._identifier._name+"'.");
				}
				else
				{
					return ErrorFunction("Unable to return multiple return values from the non aggregate return type function '"+x._identifier._name+"'.");
				}
			}

			// Validate the generated code, checking for consistency.
			if(llvm::verifyFunction(*TheFunction, llvm::VerifierFailureAction::PrintMessageAction))
			{
				return ErrorFunction("verifyFunction failure '"+x._identifier._name+"'.", EErrorLevel::Internal);
			}

			return TheFunction;

			// FIXME on error
			/*
			{
				// Error reading body, remove function.
				TheFunction->eraseFromParent();
				throw std::runtime_error("Error generating return value!");
				*/
				/*
				This code does have a bug, though. Since the PrototypeAST::Codegen can return a previously defined forward declaration, our code can actually delete a forward declaration. There are a number of ways to fix this bug, see what you can come up with! Here is a testcase:

extern foo(a b);     # ok, defines foo.
def foo(a b) c;      # error, 'c' is invalid.
def bar() foo(1, 2); # error, unknown function "foo"

				*/
			//}
		}
	}
}
/*#include <llvm/CallingConv.h>
#include <llvm/PassManager.h>
#include <llvm/Support/CallSite.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Linker.h>*/

#include <llvm/Module.h>

#include <llvm/Support/DynamicLibrary.h>
//#include <llvm/System/DynamicLibrary.h>

#include "../types.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		llvm::Function *code_generator::declare_extern(std::string name, std::vector<std::string> resTypes, std::vector<std::string> const & paramTypes, bool _bIsVarArg, void *fp)
		{
			if (fp) 
			{
				// build function declaration
				ast::function_declaration decl;
				decl._identifier._name = name;
				decl._bHasUnpureQualifier = true;
#ifdef IMPLEMENT_VAR_ARG
				decl._bIsVarArg = _bIsVarArg;
#else
				if(_bIsVarArg)
				{
					return ErrorFunction("Variable argument count not implemented during extern declaration of '"+name+"'.", EErrorLevel::Internal);
				}
#endif
				for(std::string const & x : paramTypes)
				{
					ast::identifier id(x);
					ast::type_declaration td(false, id);
					decl._parameter_types.push_back(td);
				}
				for(std::string const & x : resTypes)
				{
					ast::identifier id(x);
					ast::type_declaration td(false, id);
					decl._return_types.push_back(td);
				}

				llvm::Function * TheFunction = getModule()->getFunction(name);
				if (TheFunction)
				{
					if (llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(name) != fp)
					{
						return ErrorFunction("The extern function: '"+name+"' has been bound to a different adress before.");
					}
				}
				else
				{
					TheFunction = (*this)(decl);
					if (TheFunction == 0)
					{
						return ErrorFunction("Unable to build function declaration for extern function: '"+name+"'.");
					}

					// Enter a fixed association into the dynamic linker table.
					llvm::sys::DynamicLibrary::AddSymbol(name, fp);
				}
				return TheFunction;
			}
			else
			{
				return ErrorFunction("Extern declaration of '"+name+"' passed invalid pointer to external function.", EErrorLevel::Fatal);
			}
		}
	}
}