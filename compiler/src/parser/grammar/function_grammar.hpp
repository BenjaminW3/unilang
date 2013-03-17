#pragma once

#include "../spirit.hpp"

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
		struct type_declaration;
		struct function_declaration;
		struct variable_declaration;
		struct variable_definition;
		struct function_definition;
	}

	namespace parser
	{
		// forward declarations
		template <typename BaseIterator, typename LexerIterator>
		struct identifier_grammar;
		template <typename BaseIterator, typename LexerIterator>
		struct expression_grammar;
		template <typename BaseIterator, typename LexerIterator>
		struct statement_grammar;

		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The function grammar.
		//#########################################################################
		template <typename BaseIterator, typename LexerIterator>
		struct function_grammar : qi::grammar<LexerIterator, ast::function_definition() >
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			function_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
								identifier_grammar<BaseIterator, LexerIterator> const & identifierGrammar, 
								expression_grammar<BaseIterator, LexerIterator> const & expressionGrammar, 
								statement_grammar<BaseIterator, LexerIterator> const & statementGrammar, 
								lexer::token_lexer<BaseIterator> const & lexer);
			
			qi::rule<LexerIterator, std::vector<ast::type_declaration>()> m_ruleParameterDeclarationList;
			qi::rule<LexerIterator, std::vector<ast::type_declaration>()> m_ruleReturnDeclarationList;
			qi::rule<LexerIterator, ast::function_declaration()> m_ruleFunctionDeclaration;
			qi::rule<LexerIterator, std::vector<ast::variable_declaration>()> m_ruleParameterDefinitionList;
			qi::rule<LexerIterator, std::vector<ast::variable_definition>()> m_ruleReturnDefinitionList;
			qi::rule<LexerIterator, ast::function_definition()> m_ruleFunctionDefinition;
		};
	}
}