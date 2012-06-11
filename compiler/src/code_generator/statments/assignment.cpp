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
					if(!V->getAllocaInst())
					{
						return InternalErrorV("Variable '"+x.lhs.name+"' is not allocated!");
					}
					else
					{
						// Codegen the RHS.
						llvm::Value *Val = x.rhs.apply_visitor(*this);
						if(!Val)
						{
							return ErrorV("Invalid right hand side of an assignment!");
						}
						// ->getPointerElementType()
						if(Val->getType()!=V->getAllocaInst()->getType()->getElementType())
						{
							std::string type_str;
							llvm::raw_string_ostream rso(type_str);
							rso << "Trying to assign a value of type '";
							Val->getType()->print(rso);
							rso << "' to a value of type '";
							V->getAllocaInst()->getType()->print(rso);
							rso << "'.";
							return ErrorV("Assignment type mismatch! "+rso.str());
						}
						else
						{
							builder.CreateStore(Val, V->getAllocaInst());
							return Val;
						}
					}
				}
			}
		}
	}
}