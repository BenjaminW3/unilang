#include <unilang/compiler/code_generator/expressions/ExpressionCodeGen.hpp>

#include <unilang/compiler/code_generator/Types.hpp>
#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>
#include <unilang/compiler/code_generator/symbols/SymbolCodeGen.hpp>
#include <unilang/compiler/code_generator/symbols/Variable.hpp>

#include <unilang/compiler/ast/OperatorsFwd.hpp>
#include <unilang/compiler/ast/Expression.hpp>
#include <unilang/compiler/ast/OperatorsDef.hpp>

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

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		// This is an expression for the moment because operators::EOperators::minus_minus needs it.
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(ast::SAssignment const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// Look up the name.
			VarData const * const pVariable(m_symbolCodeGenerator.getVarFromName(x.m_lhs.m_sName));
			if(!pVariable)
			{
				return m_codeGeneratorErrors.getErrorValue("Undefined variable name '"+x.m_lhs.m_sName+"' !");
			}
			else
			{
				if(!pVariable->isMutable())
				{
					return m_codeGeneratorErrors.getErrorValue("Assignment to const (non-mutable) variable '"+x.m_lhs.m_sName+"' is impossible!");
				}
				else
				{
					llvm::AllocaInst * lhsAlloca (pVariable->getAllocaInst());
					if(!lhsAlloca)
					{
						return m_codeGeneratorErrors.getErrorValue("Variable '"+x.m_lhs.m_sName+"' is not allocated!", EErrorLevel::Internal);
					}
					else
					{
						// Codegen the RHS.
						llvm::Value *rhsVal = (*this)(x.m_rhs);
						if(!rhsVal)
						{
							return m_codeGeneratorErrors.getErrorValue("Invalid right hand side of an assignment!");
						}
						if(rhsVal->getType()!=lhsAlloca->getType()->getElementType())
						{
							return m_codeGeneratorErrors.getErrorValue("Assignment type mismatch! Trying to assign a value of type '" +getLLVMTypeName(rhsVal->getType())+ 
												"' to a value of type '"+getLLVMTypeName(lhsAlloca->getType())+"'.");
						}
						else
						{
							if(
#ifdef TOKEN_ID
				x.m_uiOperatorID
#else
				static_cast<operators::EOperators>(x.m_uiOperatorID)
#endif
								== operators::EOperators::assign)
							{
								/*return */m_llvmCodeGenerator.getBuilder()->CreateStore(rhsVal, lhsAlloca);
								return rhsVal;
							}
							else // more _thenStatementList just an assignment
							{
								llvm::Value * pVal (m_llvmCodeGenerator.getBuilder()->CreateLoad(lhsAlloca, "load.assign.lhs.val"));
								if(!pVal)
								{
									return m_codeGeneratorErrors.getErrorValue("Unable to load LHS variable value in combined assignment.");
								}

								// remove all possible flags to get the pure operation
								size_t const opRemoveFlags(~static_cast<size_t>(operators::EOperatorTypes::assignmentOperation));
								operators::EOperators const opType = static_cast<operators::EOperators>(static_cast<size_t>(x.m_uiOperatorID) & opRemoveFlags);
								llvm::Value *CalcVal = createBinaryOperation(pVal, rhsVal, opType);
								if(!CalcVal)
								{
									return m_codeGeneratorErrors.getErrorValue("Unable to compute result of operation prior to assignment!");
								}
								/*return */m_llvmCodeGenerator.getBuilder()->CreateStore(CalcVal, lhsAlloca);
								return CalcVal;
							}
						}
					}
				}
			}
		}
	}
}