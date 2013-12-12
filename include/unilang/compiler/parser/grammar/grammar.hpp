#pragma once

#include <unilang/compiler/parser/grammar/identifier_grammar.hpp>
#include <unilang/compiler/parser/grammar/function_grammar.hpp>
#include <unilang/compiler/parser/grammar/expression_grammar.hpp>
#include <unilang/compiler/parser/grammar/statement_grammar.hpp>

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
		struct meta_entity;
		struct module;
		struct namespace_declaration;
	}

	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//#########################################################################
		//!  The function grammar.
		//#########################################################################
		template <typename BaseIterator, typename LexerIterator>
		struct global_grammar : qi::grammar<LexerIterator, ast::module()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			global_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
							lexer::token_lexer<BaseIterator> const & lexer);

			// Refering to page 234 of the standard, 12.6.2, paragraph 5, the third dashed item reads:
			// "Then, non-static data members shall be initialized in the order they were declared in the class definition (again regardless of the order of the mem-initializers)."
			identifier_grammar<BaseIterator, LexerIterator> identifierGrammar;
			expression_grammar<BaseIterator, LexerIterator> expressionGrammar;
			statement_grammar<BaseIterator, LexerIterator> statementGrammar;
			function_grammar<BaseIterator, LexerIterator> functionGrammar;

			qi::rule<LexerIterator, std::vector<ast::meta_entity>()> m_rulevMetaEntities;
			qi::rule<LexerIterator, ast::module()> m_ruleModule;
			qi::rule<LexerIterator, ast::namespace_declaration()> m_ruleNamespaceDeclaration;
		};
	}
}