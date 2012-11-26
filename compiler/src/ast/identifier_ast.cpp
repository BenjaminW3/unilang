#include "identifier_ast.hpp"

namespace unilang 
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		identifier::identifier()
			:_name(/*"unnamed-identifier"*/)
		{
		}
		identifier::identifier(std::string const & name)
			:_name(name)
		{
		}
		bool identifier::isPure() const
		{
			return true;
		}
		std::ostream& operator<<(std::ostream& out, identifier const& x)
		{
			out << x._name; return out;
		}
	}
}