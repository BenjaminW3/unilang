#include <unilang/compiler/code_generator/functions/function_code_gen.hpp>

#include <unilang/compiler/code_generator/types.hpp>
#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/namespace/namespace_code_gen.hpp>
#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>
#include <unilang/compiler/code_generator/symbols/symbol_code_gen.hpp>
#include <unilang/compiler/code_generator/allocation/alloc_code_gen.hpp>
#include <unilang/compiler/code_generator/expressions/exp_code_gen.hpp>
#include <unilang/compiler/code_generator/statements/statement_code_gen.hpp>

#include <unilang/compiler/ast/function_ast.hpp>
#include <unilang/compiler/log/log.hpp>

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

#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>	// verifyFunction

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Function * function_code_generator::operator()(ast::function_definition const & x)
		{
			LOG_SCOPE_DEBUG;
			
			m_symbolCodeGenerator.clearSymbolTable();
			// TODO: symbol table
			//auto spFunctionBlockSymbolTable(m_symbolCodeGenerator.addBlock());

			// purity test
			/*if ((!x.decl._bHasUnpureQualifier) && !x._body.IsPure())
			{
				return ErrorFunction("Pure function '"+x.decl._idfName.m_sName+"' contains unpure expressions!");
			}*/
			//m_llvmCodeGenerator.getModule().get()->
  
			// build function declaration for function definition
			ast::function_declaration decl;
			decl._bHasUnpureQualifier = x._bHasUnpureQualifier;
			decl._id = x._id;
			decl._idfName = x._idfName;
			decl._vParameterDeclarations = x._vParameterDeclarations;
			ast::variable_declaration varDecl;
			varDecl._type = ast::variable_type_declaration(false, ast::identifier("i64"));	// TODO: implement non i64 variable_definition return expressions
			for( auto const & expression : x._vReturnExpressions)
			{
				decl._vReturnDeclarations.push_back(varDecl);
			}
			
			// Search for redefinition of this function.
			llvm::Function * pFunction (nullptr);
			std::string const sMangledName (m_namespaceCodeGenerator.getCurrentMangledNamespaceName()+decl.build_mangled_name());
			if(m_llvmCodeGenerator.hasFunctionFromMangledName(sMangledName))	// First test, because the getFunctionFromMangledName throws an exception in case of failure.
			{
				pFunction = m_llvmCodeGenerator.getFunctionFromMangledName(sMangledName);

				if(!pFunction->empty())
				{
					return m_codeGeneratorErrors.ErrorFunction("Redefinition of function '" + sMangledName + "' !");
				}
			}
			else
			{
				pFunction = ((*this)(decl));
				if (!pFunction)
				{
					return m_codeGeneratorErrors.ErrorFunction("Unable to build function declaration for function definition: '" + x._idfName.m_sName + "'.");
				}
			}

			// Create a new basic block to start insertion into.
			llvm::BasicBlock * const pFunctionEntryBB (llvm::BasicBlock::Create(m_llvmCodeGenerator.getContext(), "function-entry", pFunction));
			m_llvmCodeGenerator.getBuilder()->SetInsertPoint(pFunctionEntryBB);

			// add the parameters from left to right
			llvm::Function::arg_iterator itArgs (pFunction->arg_begin());
			for (size_t Idx (0), e (x._vParameterDeclarations.size()); Idx != e; ++Idx, ++itArgs)
			{
				// TODO: For now we have to define the parameters with their initialisation values and then overwrite them directly with the argument value. Maybe this can be used to add a way to ignore parameters when calling?
				llvm::AllocaInst * const pDeclAlloc (m_allocationCodeGenerator(x._vParameterDeclarations[Idx]));
				if (!pDeclAlloc)
				{
					const auto sVarName (x._vParameterDeclarations[Idx]._optionalIdentifier.is_initialized() ? " '" + x._vParameterDeclarations[Idx]._optionalIdentifier.get().m_sName + "'" : "");
					return m_codeGeneratorErrors.ErrorFunction("Unable to create parameter '" + sVarName + "' for function '" + x._idfName.m_sName + "'");
				}

				// Store the arguments in the parameter variables.
				m_llvmCodeGenerator.getBuilder()->CreateStore(itArgs, pDeclAlloc);
			}

			// Add the body.
			if(!m_statementCodeGenerator(x._body))
			{
				return m_codeGeneratorErrors.ErrorFunction("Unable to create body for function '" + x._idfName.m_sName + "'.");
			}

			// Build the return expressions.
			std::vector<llvm::Value *> vpRetValues;
			for(size_t uiReturnIndex = 0; uiReturnIndex != x._vReturnExpressions.size(); ++uiReturnIndex)
			{
				vpRetValues.push_back(m_expressionCodeGenerator(x._vReturnExpressions[uiReturnIndex]));
				if(!vpRetValues.back())
				{
					std::stringstream sstr;
					sstr << "Unable to create return expression " << uiReturnIndex << " (0-based) '" << x._vReturnExpressions[uiReturnIndex] << "' from function '" << x._idfName.m_sName << "'.";
					return m_codeGeneratorErrors.ErrorFunction(sstr.str());
				}
			}

			// Return the value(s).
			if(vpRetValues.empty())
			{
				m_llvmCodeGenerator.getBuilder()->CreateRetVoid();
			}
			else if(vpRetValues.size() == 1)
			{
				llvm::Value * const pRetVal(vpRetValues[0]);
				if(!pRetVal)
				{
					return m_codeGeneratorErrors.ErrorFunction("Unable to create load return from function '" + x._idfName.m_sName + "'.");
				}
				if(pRetVal->getType() != pFunction->getReturnType())
				{
					return m_codeGeneratorErrors.ErrorFunction("Return type mismatch! Trying to return '" + getLLVMTypeName(pRetVal->getType()) + "' from a function '" + x._idfName.m_sName + " 'returning '" + getLLVMTypeName(pFunction->getReturnType()) + "'.");
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
					//m_llvmCodeGenerator.getBuilder()->CreateAggregateRet(vpRetAllocas.data(), (unsigned int)x._vReturnExpressions.size());
					return m_codeGeneratorErrors.ErrorFunction("Unable to return multiple return values from aggregate return type function '"+x._idfName.m_sName+"'.");
				}
				else
				{
					return m_codeGeneratorErrors.ErrorFunction("Unable to return multiple return values from the non aggregate return type function '"+x._idfName.m_sName+"'.");
				}
			}

			// Validate the generated code, checking for consistency.
			std::string sError;
			llvm::raw_string_ostream rso(sError);
			if(llvm::verifyFunction(*pFunction, &rso))
			{
				return m_codeGeneratorErrors.ErrorFunction("verifyFunction failed with error: '" + rso.str() + "' in function '" + x._idfName.m_sName + "'.", EErrorLevel::Internal);
			}

			return pFunction;
		}
	}
}
/*#include <llvm/CallingConv.h>
#include <llvm/Support/CallSite.h>
*/

#include <llvm/IR/Module.h>

#include <llvm/Support/DynamicLibrary.h>
//#include <llvm/System/DynamicLibrary.h>

#include <unilang/compiler/code_generator/types.hpp>

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
				decl._idfName.m_sName = name;
				decl._bHasUnpureQualifier = true;
#ifdef IMPLEMENT_VAR_ARG
				decl._bIsVarArg = _bIsVarArg;
#else
				if(_bIsVarArg)
				{
					return m_codeGeneratorErrors.ErrorFunction("Variable argument count not implemented! Usage in extern declaration of '"+name+"'.", EErrorLevel::Internal);
				}
#endif
				for(std::string const & x : paramTypes)
				{
					ast::identifier id(x);
					ast::variable_type_declaration td(false, id);
					ast::variable_declaration var;
					var._type = td;
					decl._vParameterDeclarations.push_back(var);
				}
				for(std::string const & x : resTypes)
				{
					ast::identifier id(x);
					ast::variable_type_declaration td(false, id);
					ast::variable_declaration var;
					var._type = td;
					decl._vReturnDeclarations.push_back(var);
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