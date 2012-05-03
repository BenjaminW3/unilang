#pragma once

#include <string>

#include "../abstract_syntax_tree/abstract_syntax_tree.hpp"
#include "error_handler.hpp"

namespace unilang
{
	namespace parser
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		ast::module parse_code( std::string const & sSourceCode, error_handler<std::string::const_iterator> & error_handler );
	}
}