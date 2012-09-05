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
		llvm::Value * code_generator::operator()(ast::primary_expr const& x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		/*llvm::Value * code_generator::operator()(ast::operand const & x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}*/
		
		//-----------------------------------------------------------------------------
		// The list of operator precedences
		//-----------------------------------------------------------------------------
		int operator_precedence[] = {
            // precedence 1
            1, // op_comma

            // precedence 2
            2, // op_assign
            2, // op_plus_assign
            2, // op_minus_assign
            2, // op_times_assign
            2, // op_divide_assign
            2, // op_mod_assign
            2, // op_bit_and_assign
            2, // op_bit_xor_assign
            2, // op_bitor_assign
            2, // op_shift_left_assign
            2, // op_shift_right_assign

            // precedence 3
            3, // op_logical_or

            // precedence 4
            4, // op_logical_and

            // precedence 5
            5, // op_bit_or

            // precedence 6
            6, // op_bit_xor

            // precedence 7
            7, // op_bit_and

            // precedence 8
            8, // op_equal
            8, // op_not_equal

            // precedence 9
            9, // op_less
            9, // op_less_equal
            9, // op_greater
            9, // op_greater_equal

            // precedence 10
            10, // op_shift_left
            10, // op_shift_right

            // precedence 11
            11, // op_plus
            11, // op_minus

            // precedence 12
            12, // op_times
            12, // op_divide
            12 // op_mod
        };
		
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		int precedence_of(token_ids::type op)
		{
			return operator_precedence[op & 0xFF];
		}
		//-----------------------------------------------------------------------------
		// The Shunting-yard algorithm
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::CreateExpression(	int min_precedence,
														llvm::Value * lhs,
														std::list<ast::operation>::const_iterator & rest_begin,
														std::list<ast::operation>::const_iterator rest_end)
		{
			while ((rest_begin != rest_end) && (precedence_of(rest_begin->operator_) >= min_precedence))
			{
				token_ids::type op = rest_begin->operator_;

				llvm::Value * rhs = rest_begin->operand_.apply_visitor(*this);
				if (!rhs)
				{
					std::stringstream sstr;
					sstr << "Operand '" << rest_begin->operand_ << "' could not be evaluated!";
					return ErrorV( sstr.str() );
				}
				++rest_begin;

				while ((rest_begin != rest_end) && (precedence_of(rest_begin->operator_) > precedence_of(op)))
				{
					token_ids::type next_op = rest_begin->operator_;
					rhs = CreateExpression( precedence_of(next_op), rhs, rest_begin, rest_end);
				}

				lhs = CreateBinaryOperation(lhs, rhs, op);
			}
			return lhs;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::expression const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value * lhs = x.first.apply_visitor(*this);
			if (!lhs)
			{
				std::stringstream sstr;
				sstr << "Operand '" << x.first << "' in expression '" << x << "' could not be evaluated!";
				lhs = ErrorV( sstr.str() );
			}
			
			std::list<ast::operation>::const_iterator rest_begin = x.rest.begin();
			return CreateExpression(0, lhs, rest_begin, x.rest.end());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::CreateBinaryOperation(llvm::Value * L, llvm::Value * R, token_ids::type op)
		{
			if(!L)
			{
				return ErrorV( "Invalid L Value for operand." );
			}
			else if(!R)
			{
				return ErrorV( "Invalid R Value for operand." );
			}
			else if(L->getType() == R->getType())
			{
				if(L->getType()->isFloatingPointTy())
				{
					switch (op)
					{
					case token_ids::plus:
						{
							return builder.CreateFAdd(L, R, "add");
						}
					case token_ids::minus:
						{
							return builder.CreateFSub(L, R, "sub");
						}
					case token_ids::times:
						{
							return builder.CreateFMul(L, R, "mul");
						}
					case token_ids::divide:
						{
							return builder.CreateFDiv(L, R, "div");
						}
					case token_ids::mod:
						{
							return builder.CreateFRem(L, R, "rem");
						}
					case token_ids::equal:
						{
							return builder.CreateFCmpUEQ(L, R, "equ");
						}
					case token_ids::not_equal:
						{
							return builder.CreateFCmpUNE(L, R, "nequ");
						}
					case token_ids::less:
						{
							return builder.CreateFCmpULT(L, R, "lt");
						}
					case token_ids::less_equal:
						{
							return builder.CreateFCmpULE(L, R, "ltequ");
						}
					case token_ids::greater:
						{
							return builder.CreateFCmpUGT(L, R, "gt");
						}
					case token_ids::greater_equal:
						{
							return builder.CreateFCmpUGE(L, R, "gtequ");
						}
					case token_ids::logical_and:
						{
							return builder.CreateAnd(L, R, "and");
						}
					case token_ids::logical_or:
						{
							return builder.CreateOr(L, R, "or");
						}
					default: 
						{
							return FatalErrorV("Unknown operation!");
						}
					}
				}
				else if (L->getType()->isIntegerTy())
				{
					switch (op)
					{
					case token_ids::plus:
						{
							return builder.CreateAdd(L, R, "add");
						}
					case token_ids::minus:
						{
							return builder.CreateSub(L, R, "sub");
						}
					case token_ids::times:
						{
							return builder.CreateMul(L, R, "mul");
						}
					case token_ids::divide:
						{
							return builder.CreateSDiv(L, R, "div");
						}
					case token_ids::mod:
						{
							return builder.CreateSRem(L, R, "rem");
						}
					case token_ids::equal:
						{
							return builder.CreateICmpEQ(L, R, "equ");
						}
					case token_ids::not_equal:
						{
							return builder.CreateICmpNE(L, R, "nequ");
						}
					case token_ids::less:
						{
							return builder.CreateICmpSLT(L, R, "lt");
						}
					case token_ids::less_equal:
						{
							return builder.CreateICmpSLE(L, R, "ltequ");
						}
					case token_ids::greater:
						{
							return builder.CreateICmpSGT(L, R, "gt");
						}
					case token_ids::greater_equal:
						{
							return builder.CreateICmpSGE(L, R, "gtequ");
						}
					case token_ids::bit_and:
						{
							return builder.CreateAnd(L, R, "and");
						}
					case token_ids::bit_or:
						{
							return builder.CreateOr(L, R, "or");
						}
					case token_ids::bit_xor:
						{
							return builder.CreateXor(L, R, "xor");
						}
					/*case token_ids::compl_:
						{
							return builder.CreateOr(L, R, "or");
						}*/
					case token_ids::not_:
						{
							return builder.CreateOr(L, R, "not");
						}
					case token_ids::shift_left:
						{
							return builder.CreateOr(L, R, "shl");
						}
					case token_ids::shift_right:
						{
							return builder.CreateOr(L, R, "shr");
						}
					default: 
						{
							return FatalErrorV("Unknown operation!");
						}
					}
				}
				else
				{
					return InternalErrorV("Unsupported type in operation!");
				}
			}
			else
			{
				std::string type_str;
				llvm::raw_string_ostream rso(type_str);
				rso << "Trying to mix types '";
				L->getType()->print(rso);
				rso << "' and '";
				R->getType()->print(rso);
				rso << "' in operation.";
				return ErrorV("Operation type mismatch! "+rso.str());
			}
			
			/*case AtomicRMWInst::Nand:
				Res = Builder.CreateNot(Builder.CreateAnd(Orig, Val));
				break;
			case AtomicRMWInst::Xor:
				Res = Builder.CreateXor(Orig, Val);
				break;
			case AtomicRMWInst::Max:
				Res = Builder.CreateSelect(Builder.CreateICmpSLT(Orig, Val),
	00072                                Val, Orig);
	00073     break;
	00074   case AtomicRMWInst::Min:
	00075     Res = Builder.CreateSelect(Builder.CreateICmpSLT(Orig, Val),
	00076                                Orig, Val);
	00077     break;
	00078   case AtomicRMWInst::UMax:
	00079     Res = Builder.CreateSelect(Builder.CreateICmpULT(Orig, Val),
	00080                                Val, Orig);
	00081     break;
	00082   case AtomicRMWInst::UMin:
	00083     Res = Builder.CreateSelect(Builder.CreateICmpULT(Orig, Val),
	00084                                Orig, Val);
	00085     break;*/
		}
	}
}