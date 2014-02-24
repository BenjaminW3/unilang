#include <unilang/compiler/code_generator/expressions/exp_code_gen.hpp>

#include <unilang/compiler/code_generator/types.hpp>
#include <unilang/compiler/code_generator/errors.hpp>
#include <unilang/compiler/code_generator/llvm/llvm_code_gen.hpp>
#include <unilang/compiler/code_generator/symbols/symbol_code_gen.hpp>
#include <unilang/compiler/code_generator/symbols/symbols.hpp>

#include <unilang/compiler/ast/operators.hpp>
#include <unilang/compiler/ast/expression_ast.hpp>
#include <unilang/compiler/ast/operators_def.hpp>

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
		llvm::Value * expression_code_generator::operator()(ast::assignment const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// Look up the name.
			VarData const * const pVariable(m_symbolCodeGenerator.getVarFromName(x._lhs.m_sName));
			if(!pVariable)
			{
				return m_codeGeneratorErrors.ErrorValue("Undefined variable name '"+x._lhs.m_sName+"' !");
			}
			else
			{
				if(!pVariable->isMutable())
				{
					return m_codeGeneratorErrors.ErrorValue("Assignment to const (non-mutable) variable '"+x._lhs.m_sName+"' is impossible!");
				}
				else
				{
					llvm::AllocaInst * lhsAlloca (pVariable->getAllocaInst());
					if(!lhsAlloca)
					{
						return m_codeGeneratorErrors.ErrorValue("Variable '"+x._lhs.m_sName+"' is not allocated!", EErrorLevel::Internal);
					}
					else
					{
						// Codegen the RHS.
						llvm::Value *rhsVal = (*this)(x._rhs);
						if(!rhsVal)
						{
							return m_codeGeneratorErrors.ErrorValue("Invalid right hand side of an assignment!");
						}
						if(rhsVal->getType()!=lhsAlloca->getType()->getElementType())
						{
							return m_codeGeneratorErrors.ErrorValue("Assignment type mismatch! Trying to assign a value of type '" +getLLVMTypeName(rhsVal->getType())+ 
												"' to a value of type '"+getLLVMTypeName(lhsAlloca->getType())+"'.");
						}
						else
						{
							if(
#ifdef TOKEN_ID
				x._uiOperatorID
#else
				static_cast<operators::EOperators>(x._uiOperatorID)
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
									return m_codeGeneratorErrors.ErrorValue("Unable to load LHS variable value in combined assignment.");
								}

								// remove all possible flags to get the pure operation
								size_t const opRemoveFlags(~static_cast<size_t>(operators::EOperatorTypes::assignmentOperation));
								operators::EOperators const opType = static_cast<operators::EOperators>(static_cast<size_t>(x._uiOperatorID) & opRemoveFlags);
								llvm::Value *CalcVal = CreateBinaryOperation(pVal, rhsVal, opType);
								if(!CalcVal)
								{
									return m_codeGeneratorErrors.ErrorValue("Unable to compute result of operation prior to assignment!");
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