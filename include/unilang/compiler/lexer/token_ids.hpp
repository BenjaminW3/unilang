#pragma once

#include <ostream>

namespace unilang
{
	namespace lexer
	{
		namespace tokens
		{
			//#########################################################################
			//! Enumeration of all available token IDs.
			//! For all operations they match exactly the values from operators::EOperators.
			//#########################################################################
			enum class ETokenIDs : size_t;

			/*ETokenIDs operator | (ETokenIDs const & a, ETokenIDs const & b);
			ETokenIDs & operator |= (ETokenIDs & a, ETokenIDs const & b);
			ETokenIDs operator & (ETokenIDs const & a, ETokenIDs const & b);
			ETokenIDs & operator &= (ETokenIDs & a, ETokenIDs const & b);
			ETokenIDs operator ~ (ETokenIDs const & a);
			ETokenIDs & operator ++ (ETokenIDs & a);
			ETokenIDs operator ++ (ETokenIDs & a, int);*/

			std::ostream& operator<<(std::ostream& out, ETokenIDs const & x);
		}
	}
}