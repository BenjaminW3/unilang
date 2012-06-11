#include "../code_generator.hpp"

#include "../../log/log.hpp"

#include <boost/foreach.hpp>

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::statement const& x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * code_generator::operator()(ast::statement_list const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value * ret = nullptr;
			BOOST_FOREACH(ast::statement const& s, x)
			{
				ret=s.apply_visitor(*this);
				if (!ret)
				{
					return ErrorV("Invalid statement!");
				}
			}
			return ret;
		}
	}
}