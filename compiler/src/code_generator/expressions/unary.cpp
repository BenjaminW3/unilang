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
		llvm::Value * expression_code_generator::operator()(ast::unary_expr const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value *L = x.operand_.apply_visitor(*this);
			if(!L)
			{
				std::stringstream sstr;
				sstr << x.operand_;
				return ErrorValue("Invalid value returned from '"+sstr.str()+"'");
			}

			switch (x.operator_)
			{
			case token_ids::minus:
				{
					return builder.CreateNeg(L, "neg");
				}
			case token_ids::not_:
				{
					return builder.CreateNot(L, "not");
				}
			/*case token_ids::compl_:
				{
					llvm::Value* Minus1 = (*this)(int(-1));
					if(!Minus1)
					{
						return ErrorValue("Unable to create -1.");
					}
					return builder.CreateXor(L, Minus1, "compl.xor");
				}*/
			case token_ids::plus_plus:
				{
					return ErrorValue("plus_plus not implemented!", EErrorLevel::Fatal);
				}
			case token_ids::minus_minus:
				{
					return ErrorValue("minus_minus not implemented!", EErrorLevel::Fatal);
				}
			case token_ids::plus:
				{
					return L; // + does not change anything
				}
			case token_ids::stringify:
				{
					//throw std::runtime_error("Not implemented!");

					// FIXME: char conversion only for last digit!
					if(L->getType()->isDoubleTy())
					{
						// convert to int
						llvm::Value *pUi = builder.CreateFPToUI(L, llvm::IntegerType::get(context, 8), "FpToUiTmp");
						if(!pUi)
						{
							return ErrorValue("CreateFPToUI returned invalid value!", EErrorLevel::Internal);
						}
						// get last digit through reminder
						llvm::Value *pPosLastDigit = builder.CreateURem(pUi, (*this)(unsigned int(10)), "URemTmp");
						if(!pPosLastDigit)
						{
							return ErrorValue("CreateURem returned invalid value!", EErrorLevel::Internal);
						}
						// ascii
						return builder.CreateAdd(pPosLastDigit, (*this)(unsigned int(3*16)), "AddTmp");
					}
					else if(L->getType()->isIntegerTy())
					{
						// get last digit through reminder
						llvm::Value *pPosLastDigit = builder.CreateURem(L, (*this)(unsigned int(10)), "URemTmp");
						if(!pPosLastDigit)
						{
							return ErrorValue("CreateURem returned invalid value!", EErrorLevel::Internal);
						}
						// ascii
						return builder.CreateAdd(pPosLastDigit, (*this)(unsigned int(3*16)), "AddTmp");
					}
					else
					{
						std::string type_str;
						llvm::raw_string_ostream rso(type_str);
						rso << "String conversion for type '";
						L->getType()->print(rso);
						rso << "' not implemented!";
						return ErrorValue(rso.str(), EErrorLevel::Fatal);
					}
					/*return builder.CreateGlobalString(L, "stringifytmp");*/
				}
			default:
				{
					return ErrorValue("Unknown operation!", EErrorLevel::Fatal);
				}
			}
		}
	}
}