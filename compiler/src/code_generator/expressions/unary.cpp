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
		llvm::Value * code_generator::operator()(ast::unaryOp const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value *L = x.operand_.apply_visitor(*this);
			if(!L)
			{
				std::stringstream sstr;
				sstr << x.operand_;
				return ErrorV("Invalid value returned from '"+sstr.str()+"'");
			}

			switch (x.operator_)
			{
			case ast::op_negative:
				{
					return builder.CreateNeg(L, "negtmp");
				}
			case ast::op_not:
				{
					return builder.CreateNot(L, "nottmp");
				}
			case ast::op_positive:
				{
					return L; // + does not change anything
				}
			case ast::op_stringify:
				{
					//throw std::runtime_error("Not implemented!");

					// FIXME: char conversion only for last digit!
					if(L->getType()->isDoubleTy())
					{
						// convert to int
						llvm::Value *pUi = builder.CreateFPToUI(L, llvm::IntegerType::get(context, 8), "FpToUiTmp");
						if(!pUi)
						{
							return InternalErrorV("CreateFPToUI returned invalid value!");
						}
						// get last digit through reminder
						llvm::Value *pPosLastDigit = builder.CreateURem(pUi, (*this)(unsigned int(10)), "URemTmp");
						if(!pPosLastDigit)
						{
							return InternalErrorV("CreateURem returned invalid value!");
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
							return InternalErrorV("CreateURem returned invalid value!");
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
						return FatalErrorV(rso.str());
					}
					/*return builder.CreateGlobalString(L, "stringifytmp");*/
				}
			default:
				{
					return FatalErrorV("Unknown operation!");
				}
			}
		}
	}
}