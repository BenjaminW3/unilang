#include "../code_generator.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::identifier const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			code_generator::VarData * V = getVarFromName(x.name);
			if(!getVarFromName(x.name))
			{
				return ErrorV("Undeclared variable name: '"+x.name+"' !");
			}
			else
			{
				if(!V->getAllocaInst())
				{
					return InternalErrorV("Variable is not allocated: '"+x.name+"' !");
				}
				else
				{
					return builder.CreateLoad(V->getAllocaInst(), x.name.c_str());
				}
			}
		}
	}
}