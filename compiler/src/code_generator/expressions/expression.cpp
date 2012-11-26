#include "exp_code_gen.hpp"

#include "../../ast/operators_def.hpp"

#include "../../log/log.hpp"

#include "../types.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		// The Shunting-yard algorithm
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::CreateExpression(	size_t min_precedence,
																	llvm::Value * lhs,
																	std::list<ast::operation>::const_iterator & rest_begin,
																	std::list<ast::operation>::const_iterator rest_end)
		{
			while ((rest_begin != rest_end) && (operators::getPrecedenceOfOperator(
#ifdef TOKEN_ID
				rest_begin->_operator
#else
				static_cast<operators::EOperators>(rest_begin->_operator)
#endif
				) >= min_precedence))
			{
				operators::EOperators op = 
#ifdef TOKEN_ID
				rest_begin->_operator
#else
				static_cast<operators::EOperators>(rest_begin->_operator)
#endif
				;

				llvm::Value * rhs = rest_begin->_operand.apply_visitor(*this);
				if (!rhs)
				{
					std::stringstream sstr;
					sstr << "Operand '" << rest_begin->_operand << "' could not be evaluated!";
					return ErrorValue( sstr.str() );
				}
				++rest_begin;

				while ((rest_begin != rest_end) && (operators::getPrecedenceOfOperator(
#ifdef TOKEN_ID
				rest_begin->_operator
#else
				static_cast<operators::EOperators>(rest_begin->_operator)
#endif
				) > operators::getPrecedenceOfOperator(op)))
				{
					operators::EOperators next_op = 
#ifdef TOKEN_ID
				rest_begin->_operator
#else
				static_cast<operators::EOperators>(rest_begin->_operator)
#endif
				;
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

			llvm::Value * lhs = x._first.apply_visitor(*this);
			if (!lhs)
			{
				std::stringstream sstr;
				sstr << "Operand '" << x._first;
				if(x._rest.size()>0)
				{
					sstr << "' in expression '" << x << "' could not be evaluated!";
				}
				sstr << "' could not be evaluated!";
				lhs = ErrorValue( sstr.str() );
			}
			
			std::list<ast::operation>::const_iterator rest_begin = x._rest.begin();
			return CreateExpression(0, lhs, rest_begin, x._rest.end());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::CreateBinaryOperation(llvm::Value * L, llvm::Value * R, operators::EOperators op)
		{
			if(!L)
			{
				return ErrorValue( "Invalid L Value for operand." );
			}
			else if(!R)
			{
				return ErrorValue( "Invalid R Value for operand." );
			}
			else if(L->getType() == R->getType())
			{
				if(L->getType()->isFloatingPointTy())
				{
					switch (op)
					{
					case operators::EOperators::plus:
						{
							return builder.CreateFAdd(L, R, "add");
						}
					case operators::EOperators::minus:
						{
							return builder.CreateFSub(L, R, "sub");
						}
					case operators::EOperators::times:
						{
							return builder.CreateFMul(L, R, "mul");
						}
					case operators::EOperators::divide:
						{
							return builder.CreateFDiv(L, R, "div");
						}
					case operators::EOperators::mod:
						{
							return builder.CreateFRem(L, R, "rem");
						}
					case operators::EOperators::equal:
						{
							return builder.CreateFCmpUEQ(L, R, "equ");
						}
					case operators::EOperators::not_equal:
						{
							return builder.CreateFCmpUNE(L, R, "nequ");
						}
					case operators::EOperators::less:
						{
							return builder.CreateFCmpULT(L, R, "lt");
						}
					case operators::EOperators::less_equal:
						{
							return builder.CreateFCmpULE(L, R, "ltequ");
						}
					case operators::EOperators::greater:
						{
							return builder.CreateFCmpUGT(L, R, "gt");
						}
					case operators::EOperators::greater_equal:
						{
							return builder.CreateFCmpUGE(L, R, "gtequ");
						}
					case operators::EOperators::logical_and:
						{
							return builder.CreateAnd(L, R, "and");
						}
					case operators::EOperators::logical_or:
						{
							return builder.CreateOr(L, R, "or");
						}
					default: 
						{
							return ErrorValue("Unknown operation!", EErrorLevel::Fatal);
						}
					}
				}
				else if (L->getType()->isIntegerTy())
				{
					switch (op)
					{
					case operators::EOperators::plus:
						{
							return builder.CreateAdd(L, R, "add");
						}
					case operators::EOperators::minus:
						{
							return builder.CreateSub(L, R, "sub");
						}
					case operators::EOperators::times:
						{
							return builder.CreateMul(L, R, "mul");
						}
					case operators::EOperators::divide:
						{
							return builder.CreateSDiv(L, R, "div");
						}
					case operators::EOperators::mod:
						{
							return builder.CreateSRem(L, R, "rem");
						}
					case operators::EOperators::equal:
						{
							return builder.CreateICmpEQ(L, R, "equ");
						}
					case operators::EOperators::not_equal:
						{
							return builder.CreateICmpNE(L, R, "nequ");
						}
					case operators::EOperators::less:
						{
							return builder.CreateICmpSLT(L, R, "lt");
						}
					case operators::EOperators::less_equal:
						{
							return builder.CreateICmpSLE(L, R, "ltequ");
						}
					case operators::EOperators::greater:
						{
							return builder.CreateICmpSGT(L, R, "gt");
						}
					case operators::EOperators::greater_equal:
						{
							return builder.CreateICmpSGE(L, R, "gtequ");
						}
					case operators::EOperators::bit_and:
						{
							return builder.CreateAnd(L, R, "and");
						}
					/*case operators::EOperators::bit_nand:
						{
							return builder.CreateNot(builder.CreateAnd(L, R, "nand.and"), "nand.not");
						}*/
					case operators::EOperators::bit_or:
						{
							return builder.CreateOr(L, R, "or");
						}
					case operators::EOperators::bit_xor:
						{
							return builder.CreateXor(L, R, "xor");
						}
					/*case operators::EOperators::compl_:
						{
							return builder.CreateOr(L, R, "or");
						}*/
					case operators::EOperators::not_:
						{
							return ErrorValue("not_ not implemented!", EErrorLevel::Fatal);
							//return builder.CreateOr(L, R, "not");
						}
					case operators::EOperators::shift_left:
						{
							return builder.CreateShl(L, R, "shl");
						}
					case operators::EOperators::shift_right:
						{
							return builder.CreateLShr(L, R, "shr");
						}
					/*case operators::EOperators::max:
						{
							return builder.CreateSelect(builder.CreateICmpSLT(L, R, "min.slt"), L, R, "max.sel");
						}
					case operators::EOperators::min:
						{
							return builder.CreateSelect(builder.CreateICmpSLT(L, R, "min.slt"), R, L, "min.sel");
						}*/
					/*case operators::EOperators::umax:
						{
							return builder.CreateSelect(builder.CreateICmpULT(L, R, "min.ult"), L, R, "max.sel");
						}
					case operators::EOperators::umin:
						{
							return builder.CreateSelect(builder.CreateICmpULT(L, R, "min.ult"), R, L, "min.sel");
						}*/
					default: 
						{
							return ErrorValue("Unknown operation!", EErrorLevel::Fatal);
						}
					}
				}
				else
				{
					return ErrorValue("Unsupported type in operation!", EErrorLevel::Internal);
				}
			}
			else
			{
				return ErrorValue("Operation type mismatch! Trying to mix LHS type '"+getLLVMTypeName(L->getType())+ "' and RHS type '"+getLLVMTypeName(R->getType())+"' in operation.");
			}
		}
	}
}