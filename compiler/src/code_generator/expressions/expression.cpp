#include "exp_code_gen.hpp"

#include "../types.hpp"
#include "../errors.hpp"
#include "../llvm/llvm_code_gen.hpp"

#include "../../ast/operators.hpp"
#include "../../ast/expression_ast.hpp"
#include "../../ast/operators_def.hpp"

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

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		// The Shunting-yard algorithm
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::CreateExpression(	size_t min_precedence,
																	llvm::Value * lhs,
																	std::vector<ast::operation>::const_iterator & rest_begin,
																	std::vector<ast::operation>::const_iterator rest_end)
		{
			while ((rest_begin != rest_end) && (operators::getPrecedenceOfOperator(
#ifdef TOKEN_ID
				rest_begin->_uiOperatorID
#else
				static_cast<operators::EOperators>(rest_begin->_uiOperatorID)
#endif
				) >= min_precedence))
			{
				operators::EOperators const op ( 
#ifdef TOKEN_ID
				rest_begin->_uiOperatorID
#else
				static_cast<operators::EOperators>(rest_begin->_uiOperatorID)
#endif
				);

				llvm::Value * rhs (rest_begin->_opOperand.apply_visitor(*this));
				if (!rhs)
				{
					std::stringstream sstr;
					sstr << "Operand '" << rest_begin->_opOperand << "' could not be evaluated!";
					return m_codeGeneratorErrors.ErrorValue( sstr.str() );
				}
				++rest_begin;

				while ((rest_begin != rest_end) && (operators::getPrecedenceOfOperator(
#ifdef TOKEN_ID
				rest_begin->_uiOperatorID
#else
				static_cast<operators::EOperators>(rest_begin->_uiOperatorID)
#endif
				) > operators::getPrecedenceOfOperator(op)))
				{
					operators::EOperators const next_op ( 
#ifdef TOKEN_ID
					rest_begin->_uiOperatorID
#else
					static_cast<operators::EOperators>(rest_begin->_uiOperatorID)
#endif
					);
					rhs = CreateExpression( operators::getPrecedenceOfOperator(next_op), rhs, rest_begin, rest_end);
				}

				lhs = CreateBinaryOperation(lhs, rhs, op);
			}
			return lhs;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::expression const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value * lhs (x._firstOperand.apply_visitor(*this));
			if (!lhs)
			{
				std::stringstream sstr;
				sstr << "Operand '" << x._firstOperand;
				if(x._vRestOperands.size()>0)
				{
					sstr << "' in expression '" << x;
				}
				sstr << "' could not be evaluated!";
				lhs = m_codeGeneratorErrors.ErrorValue( sstr.str() );
			}
			
			std::vector<ast::operation>::const_iterator rest_begin (x._vRestOperands.begin());
			return CreateExpression(0, lhs, rest_begin, x._vRestOperands.end());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::CreateBinaryOperation(llvm::Value * L, llvm::Value * R, operators::EOperators op)
		{
			if(!L)
			{
				return m_codeGeneratorErrors.ErrorValue( "Invalid L Value for operand." );
			}
			else if(!R)
			{
				return m_codeGeneratorErrors.ErrorValue( "Invalid R Value for operand." );
			}
			else if(L->getType() == R->getType())
			{
				if(L->getType()->isFloatingPointTy())
				{
					switch (op)
					{
					case operators::EOperators::plus:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFAdd(L, R, "add");
						}
					case operators::EOperators::minus:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFSub(L, R, "sub");
						}
					case operators::EOperators::times:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFMul(L, R, "mul");
						}
					case operators::EOperators::divide:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFDiv(L, R, "div");
						}
					case operators::EOperators::mod:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFRem(L, R, "rem");
						}
					case operators::EOperators::equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFCmpUEQ(L, R, "equ");
						}
					case operators::EOperators::not_equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFCmpUNE(L, R, "nequ");
						}
					case operators::EOperators::less:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFCmpULT(L, R, "lt");
						}
					case operators::EOperators::less_equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFCmpULE(L, R, "ltequ");
						}
					case operators::EOperators::greater:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFCmpUGT(L, R, "gt");
						}
					case operators::EOperators::greater_equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateFCmpUGE(L, R, "gtequ");
						}
					/*case operators::EOperators::logical_and:
						{
							// FIXME both !=0
							//return m_llvmCodeGenerator.getBuilder()->CreateAnd(L, R, "and");
						}
					case operators::EOperators::logical_or:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateOr(L, R, "or");
						}*/
					default: 
						{
							std::stringstream sstr;
							sstr << "Operation '" << op;
							return m_codeGeneratorErrors.ErrorValue(sstr.str()+"' unknown for values of type '"+getLLVMTypeName(L->getType())+"'.", EErrorLevel::Fatal);
						}
					}
				}
				else if (L->getType()->isIntegerTy())
				{
					if (L->getType()->isIntegerTy() && L->getType()->getPrimitiveSizeInBits()==1)
					{
						switch (op)
						{
						case operators::EOperators::logical_and:
							{
								return m_llvmCodeGenerator.getBuilder()->CreateAnd(L, R, "and");
							}
						case operators::EOperators::logical_or:
							{
								return m_llvmCodeGenerator.getBuilder()->CreateOr(L, R, "or");
							}
						}
					}

					switch (op)
					{
					case operators::EOperators::plus:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateAdd(L, R, "add");
						}
					case operators::EOperators::minus:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateSub(L, R, "sub");
						}
					case operators::EOperators::times:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateMul(L, R, "mul");
						}
					case operators::EOperators::divide:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateSDiv(L, R, "div");
						}
					case operators::EOperators::mod:
						{
							//return m_llvmCodeGenerator.getBuilder()->CreateSRem(L, R, "rem");
							return m_llvmCodeGenerator.getBuilder()->CreateURem(L, R, "rem");
						}
					case operators::EOperators::equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateICmpEQ(L, R, "equ");
						}
					case operators::EOperators::not_equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateICmpNE(L, R, "nequ");
						}
					case operators::EOperators::less:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateICmpSLT(L, R, "lt");
						}
					case operators::EOperators::less_equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateICmpSLE(L, R, "ltequ");
						}
					case operators::EOperators::greater:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateICmpSGT(L, R, "gt");
						}
					case operators::EOperators::greater_equal:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateICmpSGE(L, R, "gtequ");
						}
					case operators::EOperators::bit_and:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateAnd(L, R, "and");
						}
					/*case operators::EOperators::bit_nand:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateNot(getBuilder()->CreateAnd(L, R, "nand.and"), "nand.not");
						}*/
					case operators::EOperators::bit_or:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateOr(L, R, "or");
						}
					case operators::EOperators::bit_xor:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateXor(L, R, "xor");
						}
					case operators::EOperators::compl:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateNeg(L, "compl");
						}
					case operators::EOperators::not_:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateNot(L, "not");
						}
					case operators::EOperators::shift_left:
						{
							// fill with zeros to the right
							return m_llvmCodeGenerator.getBuilder()->CreateShl(L, R, "shl");
						}
					case operators::EOperators::shift_right:
						{
							// logical shift right: we do not preserve the sign!
							// arithmetic shift would be AShr
							return m_llvmCodeGenerator.getBuilder()->CreateLShr(L, R, "shr");
						}
					/*case operators::EOperators::logical_and:
						{
							// FIXME both !=0
							//return m_llvmCodeGenerator.getBuilder()->CreateAnd(L, R, "and");
						}
					case operators::EOperators::logical_or:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateOr(L, R, "or");
						}*/
					/*case operators::EOperators::max:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateSelect(getBuilder()->CreateICmpSLT(L, R, "min.slt"), L, R, "max.sel");
						}
					case operators::EOperators::min:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateSelect(getBuilder()->CreateICmpSLT(L, R, "min.slt"), R, L, "min.sel");
						}*/
					/*case operators::EOperators::umax:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateSelect(getBuilder()->CreateICmpULT(L, R, "min.ult"), L, R, "max.sel");
						}
					case operators::EOperators::umin:
						{
							return m_llvmCodeGenerator.getBuilder()->CreateSelect(getBuilder()->CreateICmpULT(L, R, "min.ult"), R, L, "min.sel");
						}*/
					default: 
						{
							std::stringstream sstr;
							sstr << "Operation '" << op;
							return m_codeGeneratorErrors.ErrorValue(sstr.str()+"' unknown for values of type '"+getLLVMTypeName(L->getType())+"'.", EErrorLevel::Fatal);
						}
					}
				}
				else
				{
					return m_codeGeneratorErrors.ErrorValue("Unsupported type in operation!", EErrorLevel::Internal);
				}
			}
			else
			{
				return m_codeGeneratorErrors.ErrorValue("Operation type mismatch! Trying to mix LHS type '"+getLLVMTypeName(L->getType())+ "' and RHS type '"+getLLVMTypeName(R->getType())+"' in operation.");
			}
		}
	}
}