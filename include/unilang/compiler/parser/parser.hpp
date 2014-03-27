#pragma once

#include <string>

#include <unilang/compiler/ast/AST.hpp>

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining the parser infrastructure.
	//! 
	//! To parse code, call parseCode and use the returned ast::module.
	//-------------------------------------------------------------------------
	namespace parser
	{
		//-------------------------------------------------------------------------
		//! Parses the given source code.
		//!
		//! \param sSourceCode The source code to be parsed.
		//! \return The ast::module defining the source code.
		//-------------------------------------------------------------------------
		ast::SModule parseCode( std::string const & sSourceCode );
	}
}