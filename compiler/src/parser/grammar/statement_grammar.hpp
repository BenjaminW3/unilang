#pragma once

#include "identifier_grammar.hpp"
#include "expression_grammar.hpp"

#include "../../ast/statement_ast.hpp"

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
		//!  The statement grammar.
		//#########################################################################
		template <typename BaseIterator, typename Iterator>
		struct statement_grammar : qi::grammar<Iterator, ast::statement_list()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			statement_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
								//identifier_grammar<BaseIterator, Iterator> const & identifierGrammar, 
								expression_grammar<BaseIterator, Iterator> const & expressionGrammar, 
								lexer::token_lexer<BaseIterator>& lexer);

			qi::rule<Iterator, ast::expression()> expressionStatement;
			qi::rule<Iterator, ast::if_statement()> ifStatement;
			/*qi::rule<Iterator, ast::while_statement(), skipper<Iterator> > whileStatement;
			qi::rule<Iterator, ast::return_statement(), skipper<Iterator> > returnStatement;*/
			qi::rule<Iterator, ast::statement()> statement;

			qi::rule<Iterator, ast::statement_list()> statementList, compoundStatement;

		};
	}
}