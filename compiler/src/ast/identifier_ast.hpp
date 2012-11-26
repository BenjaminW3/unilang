#pragma once

#include "ast_base.hpp"

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
			identifier();
			identifier(std::string const & name);

			std::string _name;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, identifier const& x);
	}
}