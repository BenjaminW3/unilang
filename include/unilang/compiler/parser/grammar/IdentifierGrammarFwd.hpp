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
		struct SIdentifier;
		struct SNamespacedIdentifier;
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
		struct SIdentifierGrammar :
			boost::spirit::qi::grammar<TLexerTokenIterator, ast::SIdentifier()>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SIdentifierGrammar(
				SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
				lexer::CTokenLexer<TSourceCodeIterator> const & lexer);

			qi::rule<TLexerTokenIterator, ast::SIdentifier()> m_ruleIdentifier;
			qi::rule<TLexerTokenIterator, ast::SNamespacedIdentifier()> m_ruleNamespacedIdentifier;
		};
	}
}