#include "exp_code_gen.hpp"

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

			VarData const * const V = getVarFromName(x._name);
			if(!getVarFromName(x._name))
			{
				return ErrorValue("Undeclared variable name: '"+x._name+"' !");
			}
			else
			{
				if(!V->getAllocaInst())
				{
					return ErrorValue("Variable is not allocated: '"+x._name+"' !", EErrorLevel::Internal);
				}
				else
				{
					return builder.CreateLoad(V->getAllocaInst(), x._name.c_str());
				}
			}
		}
	}
}