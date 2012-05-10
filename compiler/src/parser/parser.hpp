#pragma once

#include <string>

#include "../ast/ast.hpp"
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