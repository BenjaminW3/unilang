#include <unilang/compiler/lexer/Literals.hpp>

#include <sstream>

namespace unilang
{
	namespace lexer
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SFloatLiteral const & x)
		{
			out << x.m_uiFullPrecisionValue;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SIntLiteral const & x)
		{
			out << x.m_uiFullPrecisionValue;
			return out;
		}
	}
}