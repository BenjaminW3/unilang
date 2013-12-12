#pragma once

#include <unilang/compiler/parser/spirit.hpp>

namespace unilang
{ 
	// forward declarations
	template <typename BaseIterator, typename LexerIterator>
	struct error_handler;
	namespace lexer
	{
		template <typename BaseIterator>
		class token_lexer;
	}
	namespace ast
	{
		struct identifier;
		struct namespaced_identifier;
	}

	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The function grammar.
		//#########################################################################
		template <typename BaseIterator, typename LexerIterator>
		struct identifier_grammar : boost::spirit::qi::grammar<LexerIterator, ast::identifier()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			identifier_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
								lexer::token_lexer<BaseIterator> const & lexer);

			qi::rule<LexerIterator, ast::identifier()> m_ruleIdentifier;
			qi::rule<LexerIterator, ast::namespaced_identifier()> m_ruleNamespacedIdentifier;
		};
	}
}