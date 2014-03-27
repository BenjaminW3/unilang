#pragma once

#include <unilang/compiler/parser/grammar/IdentifierGrammarFwd.hpp>

#include <unilang/compiler/ast/Identifier.hpp>
#include <unilang/compiler/ast/fusion_adapt/Identifier.hpp>

#include <unilang/compiler/lexer/LexerFwd.hpp>
#include <unilang/compiler/lexer/TokenIDsDef.hpp>
#include <unilang/compiler/parser/SourceErrorHandler.hpp>
#include <unilang/compiler/parser/AnnotationHandler.hpp>

#include <unilang/compiler/parser/Spirit.hpp>

namespace unilang
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;
		
		//-------------------------------------------------------------------------
		//! Constructor.
		//-------------------------------------------------------------------------
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		SIdentifierGrammar<TSourceCodeIterator,TLexerTokenIterator>::SIdentifierGrammar(
			SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
			lexer::CTokenLexer<TSourceCodeIterator> const & lexer) :
			SIdentifierGrammar::base_type(m_ruleIdentifier, "SIdentifierGrammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;
			qi::matches_type matches;
			boost::spirit::eps_type eps;

			using boost::phoenix::function;

			typedef function<unilang::SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator> > TErrorHandlerFunction;
			typedef function<unilang::SAnnotationHandler<TLexerTokenIterator> > TAnnotationFunction;

			m_ruleIdentifier = lexer.m_tokIdentifier >> eps; // struct with one element needs eps http://boost.2283326.n4.nabble.com/BOOST-FUSION-ADAPT-STRUCT-with-single-entry-td2675332.html#a2675332
			m_ruleIdentifier.name("identifier");

			m_ruleNamespacedIdentifier = 
					matches[lexer(lexer::tokens::ETokenIDs::double_colon)]
				>>	(lexer.m_tokIdentifier % lexer(lexer::tokens::ETokenIDs::double_colon));
			m_ruleNamespacedIdentifier.name("m_ruleNamespacedIdentifier");
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(m_ruleIdentifier)
				(m_ruleNamespacedIdentifier)
			);
#endif
			// Error handling: on error in start, call the error handler.
			qi::on_error<qi::fail>(m_ruleIdentifier,			TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleNamespacedIdentifier,	TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotaion.
			qi::on_success(m_ruleIdentifier,			TAnnotationFunction(sourceErrorHandler.iters)( _val, _1));
			qi::on_success(m_ruleNamespacedIdentifier,	TAnnotationFunction(sourceErrorHandler.iters)( _val, _1));
		}
	}
}