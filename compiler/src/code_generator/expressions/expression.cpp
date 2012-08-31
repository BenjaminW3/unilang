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
		llvm::Value * code_generator::operator()(ast::operand const & x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::expression const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value *L = x.first.apply_visitor(*this);
			if (!L)
			{
				std::stringstream sstr;
				sstr << "Operand '" << x.first << "' in expression '" << x << "' could not be evaluated!";
				L = ErrorV( sstr.str() );
			}

			for( ast::operation const & op : x.rest)
			{
				llvm::Value *R = op.operand_.apply_visitor(*this);
				if (!R)
				{
					std::stringstream sstr;
					sstr << "Operand '" << op.operand_ << "' in expression '" << x << "' could not be evaluated!";
					R = ErrorV( sstr.str() );
				}

				// FIXME: Type upcast possible?
				//L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context),"UItoFP");

				L = CreateOperation(L, R, op.operator_);
			}
			return L;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::CreateOperation(llvm::Value * L, llvm::Value * R, ast::optoken const & op)
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
					case ast::op_plus:
						{
							return builder.CreateFAdd(L, R, "addtmp");
						}
					case ast::op_minus:
						{
							return builder.CreateFSub(L, R, "subtmp");
						}
					case ast::op_times:
						{
							return builder.CreateFMul(L, R, "multmp");
						}
					case ast::op_divide:
						{
							return builder.CreateFDiv(L, R, "divtmp");
						}
					case ast::op_reminder:
						{
							return builder.CreateFRem(L, R, "remtmp");
						}
					case ast::op_equal:
						{
							return builder.CreateFCmpUEQ(L, R, "equtmp");
						}
					case ast::op_not_equal:
						{
							return builder.CreateFCmpUNE(L, R, "nequtmp");
						}
					case ast::op_less:
						{
							return builder.CreateFCmpULT(L, R, "lttmp");
						}
					case ast::op_less_equal:
						{
							return builder.CreateFCmpULE(L, R, "ltequtmp");
						}
					case ast::op_greater:
						{
							return builder.CreateFCmpUGT(L, R, "gttmp");
						}
					case ast::op_greater_equal:
						{
							return builder.CreateFCmpUGE(L, R, "gtequtmp");
						}
					case ast::op_and:
						{
							return builder.CreateAnd(L, R, "andtmp");
						}
					case ast::op_or:
						{
							return builder.CreateOr(L, R, "ortmp");
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
					case ast::op_plus:
						{
							return builder.CreateAdd(L, R, "addtmp");
						}
					case ast::op_minus:
						{
							return builder.CreateSub(L, R, "subtmp");
						}
					case ast::op_times:
						{
							return builder.CreateMul(L, R, "multmp");
						}
					case ast::op_divide:
						{
							return builder.CreateSDiv(L, R, "divtmp");
						}
					case ast::op_reminder:
						{
							return builder.CreateSRem(L, R, "remtmp");
						}
					case ast::op_equal:
						{
							return builder.CreateICmpEQ(L, R, "equtmp");
						}
					case ast::op_not_equal:
						{
							return builder.CreateICmpNE(L, R, "nequtmp");
						}
					case ast::op_less:
						{
							return builder.CreateICmpSLT(L, R, "lttmp");
						}
					case ast::op_less_equal:
						{
							return builder.CreateICmpSLE(L, R, "ltequtmp");
						}
					case ast::op_greater:
						{
							return builder.CreateICmpSGT(L, R, "gttmp");
						}
					case ast::op_greater_equal:
						{
							return builder.CreateICmpSGE(L, R, "gtequtmp");
						}
					case ast::op_and:
						{
							return builder.CreateAnd(L, R, "andtmp");
						}
					case ast::op_or:
						{
							return builder.CreateOr(L, R, "ortmp");
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