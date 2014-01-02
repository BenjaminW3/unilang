#pragma once

#include <unilang/compiler/parser/spirit.hpp>

#include <vector>

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
		struct primary_expr;
		struct operand;
		struct expression;
		struct function_call;
		struct variable_type_declaration;
		struct variable_declaration;
		struct variable_definition;
		struct assignment;
	}

	namespace parser
	{
		// forward declarations
		template <typename BaseIterator, typename LexerIterator>
		struct identifier_grammar;

		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The expression grammar.
		//#########################################################################
		template <typename BaseIterator, typename LexerIterator>
		struct expression_grammar :
			qi::grammar<LexerIterator, ast::expression()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			expression_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
								identifier_grammar<BaseIterator, LexerIterator> const & identifierGrammar, 
								lexer::token_lexer<BaseIterator> const & lexer);
			
			qi::rule<LexerIterator, ast::primary_expr()> m_rulePrimaryExpression;
			qi::rule<LexerIterator, ast::operand()> m_ruleUnaryExpression, m_rulePostfixExpression;
			qi::rule<LexerIterator, ast::expression()> m_ruleExpression;

			qi::rule<LexerIterator, bool()> m_ruleMutableQualifier;

			qi::rule<LexerIterator, std::vector<ast::expression>()> m_ruleArgumentList;
			qi::rule<LexerIterator, ast::function_call()> m_ruleFunctionCall;
			
			qi::rule<LexerIterator, ast::variable_type_declaration()> m_ruleTypeDeclaration;
			
			qi::rule<LexerIterator, ast::variable_declaration()> m_ruleVariableDeclaration;
			qi::rule<LexerIterator, std::vector<ast::expression>()> m_ruleDefinitionParameterList;
			qi::rule<LexerIterator, ast::variable_definition()> m_ruleVariableDefinition;

			qi::rule<LexerIterator, ast::assignment()> m_ruleAssignmentExpression;	// this is only reachable inside an assignment statement. This is no standard expression!
		};
	}
}