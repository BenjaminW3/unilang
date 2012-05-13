#pragma once

#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <string>

namespace unilang 
{
	namespace ast
	{
		//#########################################################################
		//! An Identifier.
		//#########################################################################
		struct identifier
		{
			std::string name;
		};
		inline std::ostream& operator<<(std::ostream& out, identifier const& x)
		{
			out << x.name; return out;
		}
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::identifier,
    (std::string, name)
)