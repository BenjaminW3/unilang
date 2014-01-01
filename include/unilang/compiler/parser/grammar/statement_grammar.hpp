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
		struct assignment;
		struct expression;
		struct if_statement;
		//struct while_statement;
		//struct return_statement;
		struct statement;
		struct statement_vector;
	}

	namespace parser
	{
		// forward declarations
		template <typename BaseIterator, typename LexerIterator>
		struct identifier_grammar;
		template <typename BaseIterator, typename LexerIterator>
		struct expression_grammar;

		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The statement grammar.
		//#########################################################################
		template <typename BaseIterator, typename LexerIterator>
		struct statement_grammar :
			qi::grammar<LexerIterator, ast::statement_vector()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			statement_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
								//identifier_grammar<BaseIterator, LexerIterator> const & identifierGrammar, 
								expression_grammar<BaseIterator, LexerIterator> const & expressionGrammar, 
								lexer::token_lexer<BaseIterator> const & lexer);

			qi::rule<LexerIterator, ast::expression()> m_ruleExpressionStatement;
			qi::rule<LexerIterator, ast::assignment()> m_ruleAssignmentStatement;
			qi::rule<LexerIterator, ast::if_statement()> m_ruleIfStatement;
			/*qi::rule<LexerIterator, ast::while_statement(), skipper<LexerIterator> > m_ruleWhileStatement;
			qi::rule<LexerIterator, ast::return_statement(), skipper<LexerIterator> > m_ruleReturnStatement;*/
			qi::rule<LexerIterator, ast::statement()> m_ruleStatement;

			qi::rule<LexerIterator, ast::statement_vector()> m_ruleStatementList, m_ruleCompoundStatement;

		};
	}
}