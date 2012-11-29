#include "statement_code_gen.hpp"

#include "../../ast/statement_ast.hpp"

#include "../../log/log.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::expression const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return (*dynamic_cast<expression_code_generator*>(this))(x) != nullptr;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::statement const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::statement_list const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			for(ast::statement const& s : x)
			{
				if (!s.apply_visitor(*this))
				{
					return ErrorBool("Invalid statement!");
				}
			}
			return true;
		}
	}
}