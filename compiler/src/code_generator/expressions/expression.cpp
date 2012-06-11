#include "../code_generator.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::expression const & x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}
	}
}