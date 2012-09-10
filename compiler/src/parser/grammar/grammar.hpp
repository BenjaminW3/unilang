#pragma once

#include "identifier_grammar.hpp"
#include "function_grammar.hpp"
#include "expression_grammar.hpp"
#include "statement_grammar.hpp"

#include "../../ast/ast.hpp"

#include "../spirit.hpp"

namespace unilang
{ 
	// predefinition
	template <typename BaseIterator, typename Iterator>
    struct error_handler;

	namespace lexer
	{
		// predefinition
		template <typename BaseIterator>
		struct token_lexer;
	}

	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The function grammar.
		//#########################################################################
		template <typename BaseIterator, typename Iterator>
		struct global_grammar : qi::grammar<Iterator, ast::module()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			global_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
							lexer::token_lexer<BaseIterator>& lexer);

			// Refering to page 234 of the standard, 12.6.2, paragraph 5, the third dashed item reads:
			// "Then, non-static data members shall be initialized in the order they were declared in the class definition (again regardless of the order of the mem-initializers)."
			identifier_grammar<BaseIterator, Iterator> identifierGrammar;
			expression_grammar<BaseIterator, Iterator> expressionGrammar;
			statement_grammar<BaseIterator, Iterator> statementGrammar;
			function_grammar<BaseIterator, Iterator> functionGrammar;

			qi::rule<Iterator, std::list<ast::meta_entity>()> metaEntityList;
			qi::rule<Iterator, ast::module()> module;
		};
	}
}