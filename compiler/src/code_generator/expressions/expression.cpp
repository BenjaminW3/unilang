#include "exp_code_gen.hpp"

#include "../../ast/operators_def.hpp"

#include "../../log/log.hpp"

#include "../types.hpp"

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

#include <llvm/Type.h>

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
							return getBuilder()->CreateFAdd(L, R, "add");
						}
					case operators::EOperators::minus:
						{
							return getBuilder()->CreateFSub(L, R, "sub");
						}
					case operators::EOperators::times:
						{
							return getBuilder()->CreateFMul(L, R, "mul");
						}
					case operators::EOperators::divide:
						{
							return getBuilder()->CreateFDiv(L, R, "div");
						}
					case operators::EOperators::mod:
						{
							return getBuilder()->CreateFRem(L, R, "rem");
						}
					case operators::EOperators::equal:
						{
							return getBuilder()->CreateFCmpUEQ(L, R, "equ");
						}
					case operators::EOperators::not_equal:
						{
							return getBuilder()->CreateFCmpUNE(L, R, "nequ");
						}
					case operators::EOperators::less:
						{
							return getBuilder()->CreateFCmpULT(L, R, "lt");
						}
					case operators::EOperators::less_equal:
						{
							return getBuilder()->CreateFCmpULE(L, R, "ltequ");
						}
					case operators::EOperators::greater:
						{
							return getBuilder()->CreateFCmpUGT(L, R, "gt");
						}
					case operators::EOperators::greater_equal:
						{
							return getBuilder()->CreateFCmpUGE(L, R, "gtequ");
						}
					/*case operators::EOperators::logical_and:
						{
							// FIXME both !=0
							//return getBuilder()->CreateAnd(L, R, "and");
						}
					case operators::EOperators::logical_or:
						{
							return getBuilder()->CreateOr(L, R, "or");
						}*/
					default: 
						{
							std::stringstream sstr;
							sstr << "Operation '" << op << "unknown for values of type";
							return ErrorValue(sstr.str()+getLLVMTypeName(L->getType()), EErrorLevel::Fatal);
						}
					}
				}
				else if (L->getType()->isIntegerTy())
				{
					switch (op)
					{
					case operators::EOperators::plus:
						{
							return getBuilder()->CreateAdd(L, R, "add");
						}
					case operators::EOperators::minus:
						{
							return getBuilder()->CreateSub(L, R, "sub");
						}
					case operators::EOperators::times:
						{
							return getBuilder()->CreateMul(L, R, "mul");
						}
					case operators::EOperators::divide:
						{
							return getBuilder()->CreateSDiv(L, R, "div");
						}
					case operators::EOperators::mod:
						{
							return getBuilder()->CreateSRem(L, R, "rem");
						}
					case operators::EOperators::equal:
						{
							return getBuilder()->CreateICmpEQ(L, R, "equ");
						}
					case operators::EOperators::not_equal:
						{
							return getBuilder()->CreateICmpNE(L, R, "nequ");
						}
					case operators::EOperators::less:
						{
							return getBuilder()->CreateICmpSLT(L, R, "lt");
						}
					case operators::EOperators::less_equal:
						{
							return getBuilder()->CreateICmpSLE(L, R, "ltequ");
						}
					case operators::EOperators::greater:
						{
							return getBuilder()->CreateICmpSGT(L, R, "gt");
						}
					case operators::EOperators::greater_equal:
						{
							return getBuilder()->CreateICmpSGE(L, R, "gtequ");
						}
					case operators::EOperators::bit_and:
						{
							return getBuilder()->CreateAnd(L, R, "and");
						}
					/*case operators::EOperators::bit_nand:
						{
							return getBuilder()->CreateNot(getBuilder()->CreateAnd(L, R, "nand.and"), "nand.not");
						}*/
					case operators::EOperators::bit_or:
						{
							return getBuilder()->CreateOr(L, R, "or");
						}
					case operators::EOperators::bit_xor:
						{
							return getBuilder()->CreateXor(L, R, "xor");
						}
					case operators::EOperators::compl:
						{
							return getBuilder()->CreateNeg(L, "compl");
						}
					case operators::EOperators::not_:
						{
							return getBuilder()->CreateNot(L, "not");
						}
					case operators::EOperators::shift_left:
						{
							// fill with zeros to the right
							return getBuilder()->CreateShl(L, R, "shl");
						}
					case operators::EOperators::shift_right:
						{
							// logical shift right: we do not preserve the sign!
							// arithmetic shift would be AShr
							return getBuilder()->CreateLShr(L, R, "shr");
						}
					/*case operators::EOperators::logical_and:
						{
							// FIXME both !=0
							//return getBuilder()->CreateAnd(L, R, "and");
						}
					case operators::EOperators::logical_or:
						{
							return getBuilder()->CreateOr(L, R, "or");
						}*/
					/*case operators::EOperators::max:
						{
							return getBuilder()->CreateSelect(getBuilder()->CreateICmpSLT(L, R, "min.slt"), L, R, "max.sel");
						}
					case operators::EOperators::min:
						{
							return getBuilder()->CreateSelect(getBuilder()->CreateICmpSLT(L, R, "min.slt"), R, L, "min.sel");
						}*/
					/*case operators::EOperators::umax:
						{
							return getBuilder()->CreateSelect(getBuilder()->CreateICmpULT(L, R, "min.ult"), L, R, "max.sel");
						}
					case operators::EOperators::umin:
						{
							return getBuilder()->CreateSelect(getBuilder()->CreateICmpULT(L, R, "min.ult"), R, L, "min.sel");
						}*/
					default: 
						{
							std::stringstream sstr;
							sstr << "Operation '" << op << "unknown for values of type";
							return ErrorValue(sstr.str()+getLLVMTypeName(L->getType()), EErrorLevel::Fatal);
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