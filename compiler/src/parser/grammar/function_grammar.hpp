#pragma once

#include "../spirit.hpp"

#include <list>

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
		struct type_declaration;
		struct function_declaration;
		struct variable_declaration;
		struct variable_definition;
		struct function_definition;
	}

	namespace parser
	{
		// forward declarations
		template <typename BaseIterator, typename Iterator>
		struct identifier_grammar;
		template <typename BaseIterator, typename Iterator>
		struct expression_grammar;
		template <typename BaseIterator, typename Iterator>
		struct statement_grammar;

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