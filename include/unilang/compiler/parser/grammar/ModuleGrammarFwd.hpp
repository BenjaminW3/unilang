#pragma once

#include <unilang/compiler/parser/grammar/IdentifierGrammarFwd.hpp>
#include <unilang/compiler/parser/grammar/FunctionGrammarFwd.hpp>
#include <unilang/compiler/parser/grammar/ExpressionGrammarFwd.hpp>
#include <unilang/compiler/parser/grammar/StatementGrammarFwd.hpp>

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
		struct SMetaEntity;
		struct SModule;
		struct SNamespaceDeclaration;
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
		struct SModuleGrammar :
			qi::grammar<TLexerTokenIterator, ast::SModule()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SModuleGrammar(
				SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
				lexer::CTokenLexer<TSourceCodeIterator> const & lexer);

			// Refering to page 234 of the standard, 12.6.2, paragraph 5, the third dashed item reads:
			// "Then, non-static data members shall be initialized in the order they were declared in the class definition (again regardless of the order of the mem-initializers)."
			SIdentifierGrammar<TSourceCodeIterator, TLexerTokenIterator> identifierGrammar;
			SExpressionGrammar<TSourceCodeIterator, TLexerTokenIterator> expressionGrammar;
			SStatementGrammar<TSourceCodeIterator, TLexerTokenIterator> statementGrammar;
			FunctionGrammar<TSourceCodeIterator, TLexerTokenIterator> functionGrammar;

			qi::rule<TLexerTokenIterator, std::vector<ast::SMetaEntity>()> m_rulevMetaEntities;
			qi::rule<TLexerTokenIterator, ast::SModule()> m_ruleModule;
			qi::rule<TLexerTokenIterator, ast::SNamespaceDeclaration()> m_ruleNamespaceDeclaration;
		};
	}
}