#pragma once

#include "ast_base.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

#include <ostream>
#include <string>

namespace unilang 
{
	namespace ast
	{
		//#########################################################################
		//! An Identifier.
		//#########################################################################
		struct identifier :	public ast_base
		{
			std::string name;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, identifier const& x);
	}
}

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::identifier,
	(std::string, name)
)