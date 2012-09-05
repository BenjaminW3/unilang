#pragma once

#include "identifier_grammar.hpp"

#include "../../ast/expression_ast.hpp"

#include "../spirit.hpp"

#include <list>

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
		//  The expression grammar
		//#########################################################################
		template <typename BaseIterator, typename Iterator>
		struct expression_grammar : qi::grammar<Iterator, ast::expression()>
		{
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			expression_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
								identifier_grammar<BaseIterator, Iterator> const & identifierGrammar, 
								lexer::token_lexer<BaseIterator>& lexer);

			qi::rule<Iterator, ast::primary_expr()> primary_expr;
			qi::rule<Iterator, ast::operand()>unary_expr, postfix_expr;
			qi::rule<Iterator, ast::expression()> expression;

			qi::rule<Iterator, std::list<ast::expression>()> argumentList;
			qi::rule<Iterator, ast::function_call()> functionCall;
			
			qi::rule<Iterator, bool()> mutableQualifier;
			qi::rule<Iterator, ast::type_declaration()> typeDeclaration;
			
			qi::rule<Iterator, boost::optional<ast::identifier>()> variableIdentifier;
			qi::rule<Iterator, ast::variable_declaration()> variableDeclaration;
			qi::rule<Iterator, std::list<ast::expression>()> parameterList;
			qi::rule<Iterator, ast::variable_definition()> variableDefinition;
		};
	}
}