#pragma once

#include "../spirit.hpp"

namespace unilang 
{ 
	// forward declarations
	template <typename BaseIterator, typename Iterator>
	struct error_handler;

	// forward declarations
	namespace lexer
	{
		template <typename BaseIterator>
		class token_lexer;
	}
	// forward declarations
	namespace ast
	{
		struct expression;
		struct if_statement;
		//struct while_statement;
		//struct return_statement;
		struct statement;
		struct statement_list;
	}

	namespace parser
	{
		// forward declarations
		//template <typename BaseIterator, typename Iterator>
		//struct identifier_grammar;
		template <typename BaseIterator, typename Iterator>
		struct expression_grammar;

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