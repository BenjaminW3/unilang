#pragma once

#include "identifier_grammar.hpp"
#include "statement_grammar.hpp"

#include "../../ast/function_ast.hpp"

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
		//!  The function grammar.
		//#########################################################################
		template <typename BaseIterator, typename Iterator>
		struct function_grammar : qi::grammar<Iterator, ast::function_definition() >
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			function_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
								identifier_grammar<BaseIterator, Iterator> const & identifierGrammar, 
								expression_grammar<BaseIterator, Iterator> const & expressionGrammar, 
								statement_grammar<BaseIterator, Iterator> const & statementGrammar, 
								lexer::token_lexer<BaseIterator>& lexer);
			
			qi::rule<Iterator, std::list<ast::type_declaration>()> parameterDeclarationList;
			qi::rule<Iterator, std::list<ast::type_declaration>()> returnDeclarationList;
			qi::rule<Iterator, ast::function_declaration()> functionDeclaration;
			qi::rule<Iterator, std::list<ast::variable_declaration>()> parameterDefinitionList;
			qi::rule<Iterator, std::list<ast::variable_definition>()> returnDefinitionList;
			qi::rule<Iterator, ast::function_definition()> functionDefinition;
		};
	}
}