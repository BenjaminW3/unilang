#include "identifier_ast.hpp"

namespace unilang 
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool identifier::isPure() const
		{
			return true;
		}
		std::ostream& operator<<(std::ostream& out, identifier const& x)
		{
			out << x.name; return out;
		}
	}
}