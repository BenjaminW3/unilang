#pragma once

#include <string>

#include "../ast/ast.hpp"

namespace unilang
{
	//-----------------------------------------------------------------------------
	//! The namespace defining the parser infrastructure.
	//! 
	//! To parse code, call parse_code and use the returned ast::module.
	//-----------------------------------------------------------------------------
	namespace parser
	{
		//-----------------------------------------------------------------------------
		//! Parses the given source code.
		//!
		//! \param sSourceCode The source code to be parsed.
		//! \return The ast::module defining the source code.
		//-----------------------------------------------------------------------------
		ast::module parse_code( std::string const & sSourceCode );
	}
}