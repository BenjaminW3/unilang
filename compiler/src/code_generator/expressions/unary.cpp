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
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::unary_expr const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value *L = x._operand.apply_visitor(*this);
			if(!L)
			{
				std::stringstream sstr;
				sstr << x._operand;
				return ErrorValue("Invalid value returned from '"+sstr.str()+"'");
			}

			switch (
#ifdef TOKEN_ID
				x._operator
#else
				static_cast<operators::EOperators>(x._operator)
#endif
				)
			{
			case operators::EOperators::minus:
				{
					return getBuilder()->CreateNeg(L, "neg");
				}
			case operators::EOperators::not_:
				{
					return getBuilder()->CreateNot(L, "not");
				}
			/*case operators::EOperators::compl_:
				{
					llvm::Value* Minus1 = (*this)(int(-1));
					if(!Minus1)
					{
						return ErrorValue("Unable to create -1.");
					}
					return getBuilder()->CreateXor(L, Minus1, "compl.xor");
				}*/
			case operators::EOperators::plus_plus:
				{
					if(	x._operand.var.type()==typeid(ast::primary_expr) && 
						boost::get<ast::primary_expr>(x._operand.var).var.type()==typeid(ast::identifier))
					{
						ast::expression rightEx;
						if(L->getType()->isIntegerTy())
						{
							rightEx._first = ast::operand(ast::primary_expr(size_t(1)));
						}
						else if(L->getType()->isFloatingPointTy())
						{
							rightEx._first = ast::operand(ast::primary_expr(long double(1.0)));
						}
						else
						{
							return ErrorValue("Operator '++' is not available for value of type '"+getLLVMTypeName(L->getType())+"' !", EErrorLevel::Fatal);
						}

						auto const idf (boost::get<ast::identifier>(boost::get<ast::primary_expr>(x._operand.var).var));

						ast::assignment assign	(idf,				
#ifdef TOKEN_ID
												operators::EOperators::plus_assign,
#else
												static_cast<size_t>(operators::EOperators::plus_assign),
#endif
												rightEx);
						assign._id = x._id;
						assign._lhs._id = idf._id;
						return (*this)(assign);
					}
					else
					{
						return ErrorValue("Operator '++' is only applicaple to identifiers.", EErrorLevel::Fatal);
					}
				}
			case operators::EOperators::minus_minus:
				{
					if(	x._operand.var.type()==typeid(ast::primary_expr) && 
						boost::get<ast::primary_expr>(x._operand.var).var.type()==typeid(ast::identifier))
					{
						ast::expression rightEx;
						if(L->getType()->isIntegerTy())
						{
							rightEx._first = ast::operand(ast::primary_expr(size_t(1)));
						}
						else if(L->getType()->isFloatingPointTy())
						{
							rightEx._first = ast::operand(ast::primary_expr(long double(1.0)));
						}
						else
						{
							return ErrorValue("Operator '--' is not available for value of type '"+getLLVMTypeName(L->getType())+ "' !", EErrorLevel::Fatal);
						}

						auto const idf (boost::get<ast::identifier>(boost::get<ast::primary_expr>(x._operand.var).var));

						ast::assignment assign	(idf,				
#ifdef TOKEN_ID
												operators::EOperators::plus_assign,
#else
												static_cast<size_t>(operators::EOperators::minus_assign),
#endif
												rightEx);
						assign._id = x._id;
						assign._lhs._id = idf._id;
						return (*this)(assign);
					}
					else
					{
						return ErrorValue("Operator '--' is only applicaple to identifiers.", EErrorLevel::Fatal);
					}
				}
			case operators::EOperators::plus:
				{
					return L; // + does not change anything
				}
			case operators::EOperators::stringify:
				{
					//throw std::runtime_error("Not implemented!");

					// FIXME: char conversion only for last digit!
					if(L->getType()->isDoubleTy())
					{
						// convert to int
						llvm::Value *pUi = getBuilder()->CreateFPToUI(L, llvm::IntegerType::get(getContext(), 8), "FpToUiTmp");
						if(!pUi)
						{
							return ErrorValue("CreateFPToUI returned invalid value!", EErrorLevel::Internal);
						}
						// get last digit through reminder
						llvm::Value *pPosLastDigit = getBuilder()->CreateURem(pUi, (*this)(uint64_t(10)), "URemTmp");
						if(!pPosLastDigit)
						{
							return ErrorValue("CreateURem returned invalid value!", EErrorLevel::Internal);
						}
						// ascii
						return getBuilder()->CreateAdd(pPosLastDigit, (*this)(uint64_t(3*16)), "AddTmp");
					}
					else if(L->getType()->isIntegerTy())
					{
						// get last digit through reminder
						llvm::Value *pPosLastDigit = getBuilder()->CreateURem(L, (*this)(uint64_t(10)), "URemTmp");
						if(!pPosLastDigit)
						{
							return ErrorValue("CreateURem returned invalid value!", EErrorLevel::Internal);
						}
						// ascii
						return getBuilder()->CreateAdd(pPosLastDigit, (*this)(uint64_t(3*16)), "AddTmp");
					}
					else
					{
						return ErrorValue("String conversion for type '"+getLLVMTypeName(L->getType())+"' is not implemented!", EErrorLevel::Fatal);
					}
					/*return getBuilder()->CreateGlobalString(L, "stringifytmp");*/
				}
			default:
				{
					return ErrorValue("Unknown operation!", EErrorLevel::Fatal);
				}
			}
		}
	}
}