#pragma once

#include <unilang/compiler/parser/Spirit.hpp>

#include <vector>

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
		struct SVariableDeclaration;
		struct SExpression;
		struct SFunctionDeclaration;
		struct SFunctionDefinition;
	}

	namespace parser
	{
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SIdentifierGrammar;
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SExpressionGrammar;
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SStatementGrammar;
	}
}

namespace unilang
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The function grammar.
		//#########################################################################
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct FunctionGrammar :
			qi::grammar<TLexerTokenIterator, ast::SFunctionDefinition() >
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			FunctionGrammar(
				SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
				SIdentifierGrammar<TSourceCodeIterator, TLexerTokenIterator> const & identifierGrammar, 
				SExpressionGrammar<TSourceCodeIterator, TLexerTokenIterator> const & expressionGrammar, 
				SStatementGrammar<TSourceCodeIterator, TLexerTokenIterator> const & statementGrammar, 
				lexer::CTokenLexer<TSourceCodeIterator> const & lexer);
			
			qi::rule<TLexerTokenIterator, std::vector<ast::SVariableDeclaration>()> m_ruleVariableDeclarationList;
			qi::rule<TLexerTokenIterator, ast::SFunctionDeclaration()> m_ruleFunctionDeclaration;
			qi::rule<TLexerTokenIterator, std::vector<ast::SExpression>()> m_ruleReturnDefinitionList;
			qi::rule<TLexerTokenIterator, ast::SFunctionDefinition()> m_ruleFunctionDefinition;
		};
	}
}