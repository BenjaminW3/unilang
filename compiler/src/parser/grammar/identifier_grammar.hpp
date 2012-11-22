#pragma once

#include "../../ast/identifier_ast.hpp"

#include "../spirit.hpp"

namespace unilang
{ 
	// predefinitions
	template <typename BaseIterator, typename Iterator>
	struct error_handler;

	namespace lexer
	{
		// predefinitions
		template <typename BaseIterator>
		class token_lexer;
	}

	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The function grammar.
		//#########################################################################
		template <typename BaseIterator, typename Iterator>
		struct identifier_grammar : boost::spirit::qi::grammar<Iterator, ast::identifier()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			identifier_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
								lexer::token_lexer<BaseIterator>& lexer);

			qi::rule<Iterator, ast::identifier()> identifier;
		};
	}
}