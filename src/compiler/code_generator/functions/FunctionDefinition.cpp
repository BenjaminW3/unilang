#include <unilang/compiler/code_generator/functions/CFunctionCodeGen.hpp>

#include <unilang/compiler/code_generator/Types.hpp>
#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/module/ModuleCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>
#include <unilang/compiler/code_generator/symbols/SymbolCodeGen.hpp>
#include <unilang/compiler/code_generator/allocation/AllocationCodeGen.hpp>
#include <unilang/compiler/code_generator/expressions/ExpressionCodeGen.hpp>
#include <unilang/compiler/code_generator/statements/StatementCodeGen.hpp>

#include <unilang/compiler/ast/Function.hpp>
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
		llvm::Function * CFunctionCodeGen::operator()(ast::SFunctionDefinition const & x)
		{
			LOG_SCOPE_DEBUG;
			
			m_symbolCodeGenerator.clearSymbolTable();
			// TODO: symbol table
			//auto spFunctionBlockSymbolTable(m_symbolCodeGenerator.addBlock());

			// purity test
			/*if ((!x.decl.m_bHasUnpureQualifier) && !x.m_body.IsPure())
			{
				return getErrorFunction("Pure function '"+x.decl.m_idfName.m_sName+"' contains unpure expressions!");
			}*/
			//m_llvmCodeGenerator.getModule().get()->
  
			// build function declaration for function definition
			ast::SFunctionDeclaration decl;
			decl.m_bHasUnpureQualifier = x.m_bHasUnpureQualifier;
			decl.m_uiID = x.m_uiID;
			decl.m_idfName = x.m_idfName;
			decl.m_vParameterDeclarations = x.m_vParameterDeclarations;
			ast::SVariableDeclaration varDecl;
			varDecl.m_type = ast::SVariableTypeDeclaration(false, ast::SIdentifier("i64"));	// TODO: implement non i64 SVariableDefinition return expressions
			for( auto const & expression : x.m_vReturnExpressions)
			{
				decl.m_vReturnDeclarations.push_back(varDecl);
			}
			
			// Search for redefinition of this function.
			llvm::Function * pFunction (nullptr);
			std::string const sMangledName (m_namespaceCodeGenerator.getCurrentMangledNamespaceName()+decl.build_mangled_name());
			if(m_llvmCodeGenerator.hasFunctionFromMangledName(sMangledName))	// First test, because the getFunctionFromMangledName throws an exception in case of failure.
			{
				pFunction = m_llvmCodeGenerator.getFunctionFromMangledName(sMangledName);

				if(!pFunction->empty())
				{
					return m_codeGeneratorErrors.getErrorFunction("Redefinition of function '" + sMangledName + "' !");
				}
			}
			else
			{
				pFunction = ((*this)(decl));
				if (!pFunction)
				{
					return m_codeGeneratorErrors.getErrorFunction("Unable to build function declaration for function definition: '" + x.m_idfName.m_sName + "'.");
				}
			}

			// Create a new basic block to start insertion into.
			llvm::BasicBlock * const pFunctionEntryBB (llvm::BasicBlock::Create(m_llvmCodeGenerator.getContext(), "function-entry", pFunction));
			m_llvmCodeGenerator.getBuilder()->SetInsertPoint(pFunctionEntryBB);

			// add the parameters from left to right
			llvm::Function::arg_iterator itArgs (pFunction->arg_begin());
			for (size_t Idx (0), e (x.m_vParameterDeclarations.size()); Idx != e; ++Idx, ++itArgs)
			{
				// TODO: For now we have to define the parameters with their initialisation values and then overwrite them directly with the argument value. Maybe this can be used to add a way to ignore parameters when calling?
				llvm::AllocaInst * const pDeclAlloc (m_allocationCodeGenerator(x.m_vParameterDeclarations[Idx]));
				if (!pDeclAlloc)
				{
					const auto sVarName (x.m_vParameterDeclarations[Idx].m_optionalIdentifier.is_initialized() ? " '" + x.m_vParameterDeclarations[Idx].m_optionalIdentifier.get().m_sName + "'" : "");
					return m_codeGeneratorErrors.getErrorFunction("Unable to create parameter '" + sVarName + "' for function '" + x.m_idfName.m_sName + "'");
				}

				// Store the arguments in the parameter variables.
				m_llvmCodeGenerator.getBuilder()->CreateStore(itArgs, pDeclAlloc);
			}

			// Add the body.
			if(!m_statementCodeGenerator(x.m_body))
			{
				return m_codeGeneratorErrors.getErrorFunction("Unable to create body for function '" + x.m_idfName.m_sName + "'.");
			}

			// Build the return expressions.
			std::vector<llvm::Value *> vpRetValues;
			for(size_t uiReturnIndex = 0; uiReturnIndex != x.m_vReturnExpressions.size(); ++uiReturnIndex)
			{
				vpRetValues.push_back(m_expressionCodeGenerator(x.m_vReturnExpressions[uiReturnIndex]));
				if(!vpRetValues.back())
				{
					std::stringstream sstr;
					sstr << "Unable to create return expression " << uiReturnIndex << " (0-based) '" << x.m_vReturnExpressions[uiReturnIndex] << "' from function '" << x.m_idfName.m_sName << "'.";
					return m_codeGeneratorErrors.getErrorFunction(sstr.str());
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
					return m_codeGeneratorErrors.getErrorFunction("Unable to create load return from function '" + x.m_idfName.m_sName + "'.");
				}
				if(pRetVal->getType() != pFunction->getReturnType())
				{
					return m_codeGeneratorErrors.getErrorFunction("Return type mismatch! Trying to return '" + getLLVMTypeName(pRetVal->getType()) + "' from a function '" + x.m_idfName.m_sName + " 'returning '" + getLLVMTypeName(pFunction->getReturnType()) + "'.");
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
					//m_llvmCodeGenerator.getBuilder()->CreateAggregateRet(vpRetAllocas.data(), (unsigned int)x.m_vReturnExpressions.size());
					return m_codeGeneratorErrors.getErrorFunction("Unable to return multiple return values from aggregate return type function '"+x.m_idfName.m_sName+"'.");
				}
				else
				{
					return m_codeGeneratorErrors.getErrorFunction("Unable to return multiple return values from the non aggregate return type function '"+x.m_idfName.m_sName+"'.");
				}
			}

			// Validate the generated code, checking for consistency.
			std::string sError;
			llvm::raw_string_ostream rso(sError);
			if(llvm::verifyFunction(*pFunction, &rso))
			{
				return m_codeGeneratorErrors.getErrorFunction("verifyFunction failed with error: '" + rso.str() + "' in function '" + x.m_idfName.m_sName + "'.", EErrorLevel::Internal);
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

#include <unilang/compiler/code_generator/Types.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		llvm::Function *CFunctionCodeGen::declare_extern(std::string const & name, std::vector<std::string> const & resTypes, std::vector<std::string> const & paramTypes, bool _bIsVarArg, void * const fp)
		{
			if (fp) 
			{
				// build function declaration
				ast::SFunctionDeclaration decl;
				decl.m_idfName.m_sName = name;
				decl.m_bHasUnpureQualifier = true;
#ifdef IMPLEMENT_VAR_ARG
				decl._bIsVarArg = _bIsVarArg;
#else
				if(_bIsVarArg)
				{
					return m_codeGeneratorErrors.getErrorFunction("Variable argument count not implemented! Usage in extern declaration of '"+name+"'.", EErrorLevel::Internal);
				}
#endif
				for(std::string const & x : paramTypes)
				{
					ast::SIdentifier id(x);
					ast::SVariableTypeDeclaration td(false, id);
					ast::SVariableDeclaration var;
					var.m_type = td;
					decl.m_vParameterDeclarations.push_back(var);
				}
				for(std::string const & x : resTypes)
				{
					ast::SIdentifier id(x);
					ast::SVariableTypeDeclaration td(false, id);
					ast::SVariableDeclaration var;
					var.m_type = td;
					decl.m_vReturnDeclarations.push_back(var);
				}

				llvm::Function * pExternFunction (m_llvmCodeGenerator.getModule()->getFunction(name));
				if (pExternFunction)
				{
					if (llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(name) != fp)
					{
						return m_codeGeneratorErrors.getErrorFunction("The extern function: '"+name+"' has been bound to a different adress before.");
					}
				}
				else
				{
					pExternFunction = (*this)(decl);
					if (pExternFunction == nullptr)
					{
						return m_codeGeneratorErrors.getErrorFunction("Unable to build function declaration for extern function: '"+name+"'.");
					}

					// Enter a fixed association into the dynamic linker table.
					llvm::sys::DynamicLibrary::AddSymbol(name, fp);
				}
				return pExternFunction;
			}
			else
			{
				return m_codeGeneratorErrors.getErrorFunction("Extern declaration of '"+name+"' passed invalid pointer to external function.", EErrorLevel::Fatal);
			}
		}
	}
}