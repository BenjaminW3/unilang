#pragma once

#include <unilang/compiler/parser/Spirit.hpp>

// forward declarations
namespace unilang
{
	template <typename TSourceCodeIterator, typename TLexerTokenIterator>
	struct SSourceErrorHandler;
	namespace lexer
	{
		template <typename TSourceCodeIterator>
		class CTokenLexer;
	}
	namespace ast
	{
		struct SAssignment;
		struct SExpression;
		struct SIfStatement;
		//struct SWhileStatement;
		//struct SReturnStatement;
		struct SStatement;
		struct SStatementVector;
	}

	namespace parser
	{
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SIdentifierGrammar;
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SExpressionGrammar;
	}
}

namespace unilang
{
	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The statement grammar.
		//#########################################################################
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SStatementGrammar :
			qi::grammar<TLexerTokenIterator, ast::SStatementVector()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SStatementGrammar(
				SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
				//SIdentifierGrammar<TSourceCodeIterator, TLexerTokenIterator> const & identifierGrammar, 
				SExpressionGrammar<TSourceCodeIterator, TLexerTokenIterator> const & expressionGrammar, 
				lexer::CTokenLexer<TSourceCodeIterator> const & lexer);

			qi::rule<TLexerTokenIterator, ast::SExpression()> m_ruleExpressionStatement;
			qi::rule<TLexerTokenIterator, ast::SAssignment()> m_ruleAssignmentStatement;
			qi::rule<TLexerTokenIterator, ast::SIfStatement()> m_ruleIfStatement;
			/*qi::rule<TLexerTokenIterator, ast::SWhileStatement(), skipper<TLexerTokenIterator> > m_ruleWhileStatement;
			qi::rule<TLexerTokenIterator, ast::SReturnStatement(), skipper<TLexerTokenIterator> > m_ruleReturnStatement;*/
			qi::rule<TLexerTokenIterator, ast::SStatement()> m_ruleStatement;

			qi::rule<TLexerTokenIterator, ast::SStatementVector()> m_ruleStatementList, m_ruleCompoundStatement;

		};
	}
}