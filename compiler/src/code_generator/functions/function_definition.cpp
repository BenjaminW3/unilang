#include "function_code_gen.hpp"

#include "../types.hpp"
#include "../errors.hpp"
#include "../namespace/namespace_code_gen.hpp"
#include "../llvm/llvm_code_gen.hpp"
#include "../symbols/symbol_code_gen.hpp"
#include "../allocation/alloc_code_gen.hpp"
#include "../statements/statement_code_gen.hpp"

#include "../../ast/function_ast.hpp"
#include "../../log/log.hpp"

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

#include <llvm/Analysis/Verifier.h>
#include <llvm/IRBuilder.h>
#include <llvm/Function.h>
#include <llvm/Type.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function * function_code_generator::operator()(ast::function_definition const & x)
		{
			LOG_SCOPE_DEBUG;
			
			// TODO: symbol table
			m_symbolCodeGenerator.vSymbolTable.clear();

			// purity test
			/*if ((!x.decl._bHasUnpureQualifier) && !x._body.IsPure())
			{
				return ErrorFunction("Pure function '"+x.decl._idfName._name+"' contains unpure expressions!");
			}*/
			//m_llvmCodeGenerator.getModule().get()->
  
			// build function declaration for function definition
			ast::function_declaration decl(x);
			
			llvm::Function * pFunction (nullptr);
			std::string const sMangledName (m_namespaceCodeGenerator.getCurrentMangledNamespaceName()+decl.build_mangled_name());
			if(m_llvmCodeGenerator.hasFunctionFromMangledName(sMangledName))
			{
				pFunction = m_llvmCodeGenerator.getFunctionFromMangledName(sMangledName);

				if(!pFunction->empty())
				{
					return m_codeGeneratorErrors.ErrorFunction("Redefinition of function '"+sMangledName+"' !");
				}
			}
			else
			{
				pFunction = ((*this)(decl));
				if (!pFunction)
				{
					return m_codeGeneratorErrors.ErrorFunction("Unable to build function declaration for function definition: '"+x._idfName._name+"'.");
				}
			}

			// Create a new basic block to start insertion into.
			llvm::BasicBlock * const BB (llvm::BasicBlock::Create(m_llvmCodeGenerator.getContext(), "entry", pFunction));
			m_llvmCodeGenerator.getBuilder()->SetInsertPoint(BB);

			// add the parameters from left to right
			llvm::Function::arg_iterator itArgs (pFunction->arg_begin());
			for (size_t Idx (0), e (x._vParameterDeclarations.size()); Idx != e; ++Idx, ++itArgs)
			{
				// TODO: For now we have to define the parameters with their initialisation values and then overwrite them directly with the argument value. Maybe this can be used to add a way to ignore parameters when calling?
				llvm::AllocaInst * const pDeclAlloc (m_allocationCodeGenerator(x._vParameterDeclarations[Idx]));
				if (!pDeclAlloc)
				{
					const auto sVarName (x._vParameterDeclarations[Idx]._optionalIdentifier.is_initialized() ? " '"+x._vParameterDeclarations[Idx]._optionalIdentifier.get()._name+"'" : "");
					return m_codeGeneratorErrors.ErrorFunction("Unable to create parameter '"+ sVarName+"' for function '"+x._idfName._name+"'");
				}

				// store the arguments in the parameter variables
				m_llvmCodeGenerator.getBuilder()->CreateStore(itArgs, pDeclAlloc);
			}

			// add the return values
			std::vector<llvm::AllocaInst *> vpRetAllocas;
			for (unsigned int Idx = 0; Idx != x._vReturnValueDefinitions.size(); ++Idx)
			{
				vpRetAllocas.push_back(m_allocationCodeGenerator(x._vReturnValueDefinitions[Idx]));
				if(!vpRetAllocas.back())
				{
					const auto sVarName (x._vReturnValueDefinitions[Idx]._declaration._optionalIdentifier.is_initialized() ? " '"+x._vReturnValueDefinitions[Idx]._declaration._optionalIdentifier.get()._name+"'" : "");
					return m_codeGeneratorErrors.ErrorFunction("Unable to create return value '"+ sVarName+"' from function '"+x._idfName._name+"'");
				}
			}

			// add body
			if(!m_statementCodeGenerator(x._body))
			{
				return m_codeGeneratorErrors.ErrorFunction("Unable to create body for function '"+x._idfName._name+"'.");
			}

			// return value(s)
			if(vpRetAllocas.empty())
			{
				m_llvmCodeGenerator.getBuilder()->CreateRetVoid();
			}
			else if(vpRetAllocas.size() == 1)
			{
				llvm::Value * const pRetVal (m_llvmCodeGenerator.getBuilder()->CreateLoad(vpRetAllocas[0], "loadret"));
				if(!pRetVal)
				{
					return m_codeGeneratorErrors.ErrorFunction("Unable to create load return from function '"+x._idfName._name+"'.");
				}
				if(pRetVal->getType() != pFunction->getReturnType())
				{
					return m_codeGeneratorErrors.ErrorFunction("Return type mismatch! Trying to return '"+getLLVMTypeName(pRetVal->getType())+"' from a function '"+x._idfName._name+" 'returning '"+getLLVMTypeName(pFunction->getReturnType())+"'.");
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
				m_llvmCodeGenerator.getBuilder()->CreateRet(pRetVal);
			}
			else
			{
				if(pFunction->getReturnType()->isAggregateType())
				{
					// FIXME: handle multi-return value functions in result expressions.
					// FIXME: not alloca, instead values
					//m_llvmCodeGenerator.getBuilder()->CreateAggregateRet(vpRetAllocas.data(), (unsigned int)x._vReturnValueDefinitions.size());
					return m_codeGeneratorErrors.ErrorFunction("Unable to return multiple return values from aggregate return type function '"+x._idfName._name+"'.");
				}
				else
				{
					return m_codeGeneratorErrors.ErrorFunction("Unable to return multiple return values from the non aggregate return type function '"+x._idfName._name+"'.");
				}
			}

			// Validate the generated code, checking for consistency.
			if(llvm::verifyFunction(*pFunction, llvm::VerifierFailureAction::PrintMessageAction))
			{
				return m_codeGeneratorErrors.ErrorFunction("verifyFunction failure '"+x._idfName._name+"'.", EErrorLevel::Internal);
			}

			return pFunction;
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
		llvm::Function *function_code_generator::declare_extern(std::string const & name, std::vector<std::string> const & resTypes, std::vector<std::string> const & paramTypes, bool _bIsVarArg, void * const fp)
		{
			if (fp) 
			{
				// build function declaration
				ast::function_declaration decl;
				decl._idfName._name = name;
				decl._bHasUnpureQualifier = true;
#ifdef IMPLEMENT_VAR_ARG
				decl._bIsVarArg = _bIsVarArg;
#else
				if(_bIsVarArg)
				{
					return m_codeGeneratorErrors.ErrorFunction("Variable argument count not implemented during extern declaration of '"+name+"'.", EErrorLevel::Internal);
				}
#endif
				for(std::string const & x : paramTypes)
				{
					ast::identifier id(x);
					ast::type_declaration td(false, id);
					decl._vParameterTypes.push_back(td);
				}
				for(std::string const & x : resTypes)
				{
					ast::identifier id(x);
					ast::type_declaration td(false, id);
					decl._vReturnTypes.push_back(td);
				}

				llvm::Function * pExternFunction (m_llvmCodeGenerator.getModule()->getFunction(name));
				if (pExternFunction)
				{
					if (llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(name) != fp)
					{
						return m_codeGeneratorErrors.ErrorFunction("The extern function: '"+name+"' has been bound to a different adress before.");
					}
				}
				else
				{
					pExternFunction = (*this)(decl);
					if (pExternFunction == nullptr)
					{
						return m_codeGeneratorErrors.ErrorFunction("Unable to build function declaration for extern function: '"+name+"'.");
					}

					// Enter a fixed association into the dynamic linker table.
					llvm::sys::DynamicLibrary::AddSymbol(name, fp);
				}
				return pExternFunction;
			}
			else
			{
				return m_codeGeneratorErrors.ErrorFunction("Extern declaration of '"+name+"' passed invalid pointer to external function.", EErrorLevel::Fatal);
			}
		}
	}
}