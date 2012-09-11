#include "../code_generator.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::identifier const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			VarData * V = getVarFromName(x.name);
			if(!getVarFromName(x.name))
			{
				return ErrorValue("Undeclared variable name: '"+x.name+"' !");
			}
			else
			{
				if(!V->getAllocaInst())
				{
					return ErrorValue("Variable is not allocated: '"+x.name+"' !", EErrorLevel::Internal);
				}
				else
				{
					return builder.CreateLoad(V->getAllocaInst(), x.name.c_str());
				}
			}
		}
	}
}