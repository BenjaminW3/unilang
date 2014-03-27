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
		struct SPrimaryExpr;
		struct SOperand;
		struct SExpression;
		struct SFunctionCall;
		struct SVariableTypeDeclaration;
		struct SVariableDeclaration;
		struct SVariableDefinition;
		struct SAssignment;
	}
	namespace parser
	{
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SIdentifierGrammar;
	}
}

namespace unilang
{
	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The expression grammar.
		//#########################################################################
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		struct SExpressionGrammar :
			qi::grammar<TLexerTokenIterator, ast::SExpression()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SExpressionGrammar(
				SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler,
				SIdentifierGrammar<TSourceCodeIterator, TLexerTokenIterator> const & identifierGrammar, 
				lexer::CTokenLexer<TSourceCodeIterator> const & lexer);
			
			qi::rule<TLexerTokenIterator, ast::SPrimaryExpr()> m_rulePrimaryExpression;
			qi::rule<TLexerTokenIterator, ast::SOperand()> m_ruleUnaryExpression, m_rulePostfixExpression;
			qi::rule<TLexerTokenIterator, ast::SExpression()> m_ruleExpression;

			qi::rule<TLexerTokenIterator, bool()> m_ruleMutableQualifier;

			qi::rule<TLexerTokenIterator, std::vector<ast::SExpression>()> m_ruleArgumentList;
			qi::rule<TLexerTokenIterator, ast::SFunctionCall()> m_ruleFunctionCall;
			
			qi::rule<TLexerTokenIterator, ast::SVariableTypeDeclaration()> m_ruleTypeDeclaration;
			
			qi::rule<TLexerTokenIterator, ast::SVariableDeclaration()> m_ruleVariableDeclaration;
			qi::rule<TLexerTokenIterator, std::vector<ast::SExpression>()> m_ruleDefinitionParameterList;
			qi::rule<TLexerTokenIterator, ast::SVariableDefinition()> m_ruleVariableDefinition;

			qi::rule<TLexerTokenIterator, ast::SAssignment()> m_ruleAssignmentExpression;	// this is only reachable inside an assignment statement. This is no standard expression!
		};
	}
}