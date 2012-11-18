#include "../code_generator.hpp"

#include "../../log/log.hpp"

#include "llvm/Support/raw_ostream.h"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::primary_expr const& x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		/*llvm::Value * expression_code_generator::operator()(ast::operand const & x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}*/
		
		//-----------------------------------------------------------------------------
		// The Shunting-yard algorithm
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::CreateExpression(	size_t min_precedence,
														llvm::Value * lhs,
														std::list<ast::operation>::const_iterator & rest_begin,
														std::list<ast::operation>::const_iterator rest_end)
		{
			while ((rest_begin != rest_end) && (op::getPrecedenceOfOperator(rest_begin->operator_) >= min_precedence))
			{
				op::types op = rest_begin->operator_;

				llvm::Value * rhs = rest_begin->operand_.apply_visitor(*this);
				if (!rhs)
				{
					std::stringstream sstr;
					sstr << "Operand '" << rest_begin->operand_ << "' could not be evaluated!";
					return ErrorValue( sstr.str() );
				}
				++rest_begin;

				while ((rest_begin != rest_end) && (op::getPrecedenceOfOperator(rest_begin->operator_) > op::getPrecedenceOfOperator(op)))
				{
					op::types next_op = rest_begin->operator_;
					rhs = CreateExpression( op::getPrecedenceOfOperator(next_op), rhs, rest_begin, rest_end);
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

			llvm::Value * lhs = x.first.apply_visitor(*this);
			if (!lhs)
			{
				std::stringstream sstr;
				sstr << "Operand '" << x.first << "' in expression '" << x << "' could not be evaluated!";
				lhs = ErrorValue( sstr.str() );
			}
			
			std::list<ast::operation>::const_iterator rest_begin = x.rest.begin();
			return CreateExpression(0, lhs, rest_begin, x.rest.end());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::CreateBinaryOperation(llvm::Value * L, llvm::Value * R, op::types op)
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
					case op::plus:
						{
							return builder.CreateFAdd(L, R, "add");
						}
					case op::minus:
						{
							return builder.CreateFSub(L, R, "sub");
						}
					case op::times:
						{
							return builder.CreateFMul(L, R, "mul");
						}
					case op::divide:
						{
							return builder.CreateFDiv(L, R, "div");
						}
					case op::mod:
						{
							return builder.CreateFRem(L, R, "rem");
						}
					case op::equal:
						{
							return builder.CreateFCmpUEQ(L, R, "equ");
						}
					case op::not_equal:
						{
							return builder.CreateFCmpUNE(L, R, "nequ");
						}
					case op::less:
						{
							return builder.CreateFCmpULT(L, R, "lt");
						}
					case op::less_equal:
						{
							return builder.CreateFCmpULE(L, R, "ltequ");
						}
					case op::greater:
						{
							return builder.CreateFCmpUGT(L, R, "gt");
						}
					case op::greater_equal:
						{
							return builder.CreateFCmpUGE(L, R, "gtequ");
						}
					case op::logical_and:
						{
							return builder.CreateAnd(L, R, "and");
						}
					case op::logical_or:
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
					case op::plus:
						{
							return builder.CreateAdd(L, R, "add");
						}
					case op::minus:
						{
							return builder.CreateSub(L, R, "sub");
						}
					case op::times:
						{
							return builder.CreateMul(L, R, "mul");
						}
					case op::divide:
						{
							return builder.CreateSDiv(L, R, "div");
						}
					case op::mod:
						{
							return builder.CreateSRem(L, R, "rem");
						}
					case op::equal:
						{
							return builder.CreateICmpEQ(L, R, "equ");
						}
					case op::not_equal:
						{
							return builder.CreateICmpNE(L, R, "nequ");
						}
					case op::less:
						{
							return builder.CreateICmpSLT(L, R, "lt");
						}
					case op::less_equal:
						{
							return builder.CreateICmpSLE(L, R, "ltequ");
						}
					case op::greater:
						{
							return builder.CreateICmpSGT(L, R, "gt");
						}
					case op::greater_equal:
						{
							return builder.CreateICmpSGE(L, R, "gtequ");
						}
					case op::bit_and:
						{
							return builder.CreateAnd(L, R, "and");
						}
					/*case op::bit_nand:
						{
							return builder.CreateNot(builder.CreateAnd(L, R, "nand.and"), "nand.not");
						}*/
					case op::bit_or:
						{
							return builder.CreateOr(L, R, "or");
						}
					case op::bit_xor:
						{
							return builder.CreateXor(L, R, "xor");
						}
					/*case op::compl_:
						{
							return builder.CreateOr(L, R, "or");
						}*/
					case op::not_:
						{
							return ErrorValue("not_ not implemented!", EErrorLevel::Fatal);
							//return builder.CreateOr(L, R, "not");
						}
					case op::shift_left:
						{
							return builder.CreateShl(L, R, "shl");
						}
					case op::shift_right:
						{
							return builder.CreateLShr(L, R, "shr");
						}
					/*case op::max:
						{
							return builder.CreateSelect(builder.CreateICmpSLT(L, R, "min.slt"), L, R, "max.sel");
						}
					case op::min:
						{
							return builder.CreateSelect(builder.CreateICmpSLT(L, R, "min.slt"), R, L, "min.sel");
						}*/
					/*case op::umax:
						{
							return builder.CreateSelect(builder.CreateICmpULT(L, R, "min.ult"), L, R, "max.sel");
						}
					case op::umin:
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
				std::string type_str;
				llvm::raw_string_ostream rso(type_str);
				rso << "Trying to mix type '";
				L->getType()->print(rso);
				rso << "' and '";
				R->getType()->print(rso);
				rso << "' in operation.";
				return ErrorValue("Operation type mismatch! "+rso.str());
			}
		}
	}
}