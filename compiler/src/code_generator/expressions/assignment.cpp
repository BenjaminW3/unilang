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
		llvm::Value * code_generator::operator()(ast::assignment const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// Look up the name.
			code_generator::VarData * V = getVarFromName(x.lhs.name);
			if(!V)
			{
				return ErrorV("Undefined variable name '"+x.lhs.name+"' !");
			}
			else
			{
				if(!V->isMutable())
				{
					return ErrorV("Assignment to const (non-mutable) variable '"+x.lhs.name+"' is impossible!");
				}
				else
				{
					llvm::AllocaInst *lhs = V->getAllocaInst();
					if(!lhs)
					{
						return InternalErrorV("Variable '"+x.lhs.name+"' is not allocated!");
					}
					else
					{
						// Codegen the RHS.
						llvm::Value *rhs = (*this)(x.rhs);
						if(!rhs)
						{
							return ErrorV("Invalid right hand side of an assignment!");
						}
						if(rhs->getType()!=lhs->getType()->getElementType())
						{
							std::string type_str;
							llvm::raw_string_ostream rso(type_str);
							rso << "Trying to assign a value of type '";
							rhs->getType()->print(rso);
							rso << "' to a value of type '";
							lhs->getType()->print(rso);
							rso << "'.";
							return ErrorV("Assignment type mismatch! "+rso.str());
						}
						else
						{
							if(x.operator_ == token_ids::assign)
							{
								/*return */builder.CreateStore(rhs, lhs);
								return rhs;
							}
							else // more then just an assignment
							{
								ast::operation op;
								llvm::Value *CalcVal = CreateBinaryOperation(lhs, rhs, x.operator_);
								if(!CalcVal)
								{
									return ErrorV("Unable to compute result of operation prior to assignment!");
								}
								/*return */builder.CreateStore(CalcVal, lhs);
								return CalcVal;
							}
						}
					}
				}
			}
		}
	}
}