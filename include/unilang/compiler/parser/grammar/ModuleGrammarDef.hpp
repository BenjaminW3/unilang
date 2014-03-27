#pragma once

#include <unilang/compiler/parser/grammar/ModuleGrammarFwd.hpp>

#include <unilang/compiler/ast/AST.hpp>
#include <unilang/compiler/ast/fusion_adapt/AST.hpp>

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
		SModuleGrammar<TSourceCodeIterator,TLexerTokenIterator>::SModuleGrammar(
			SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
			lexer::CTokenLexer<TSourceCodeIterator> const & lexer) :
			SModuleGrammar::base_type(m_ruleModule, "ModuleGrammar"),
			identifierGrammar(sourceErrorHandler, lexer),
			expressionGrammar(sourceErrorHandler, identifierGrammar, lexer),
			statementGrammar(sourceErrorHandler, /*identifierGrammar,*/ expressionGrammar, lexer),
			functionGrammar(sourceErrorHandler, identifierGrammar, expressionGrammar, statementGrammar, lexer)
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			using boost::phoenix::function;

			typedef function<unilang::SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator> > TErrorHandlerFunction;
			typedef function<unilang::SAnnotationHandler<TLexerTokenIterator> > TAnnotationFunction;

			// TODO: Benchmark on huge codebases which order is faster
			// Version 1:
			// Sort by number of occurences but possibly more backtracking from namespaces read as functions.
			// But because functions are by far more frequent most of the time it is a direct hit.
			m_rulevMetaEntities = *(functionGrammar | functionGrammar.m_ruleFunctionDeclaration | m_ruleNamespaceDeclaration);
			// Version 2:
			// 1. namespace				because it has to match on first lexer token.
			// 2. Function declaration					- " -
			// -> On average 2 backtracks because functions are by far more frequent.
			//m_rulevMetaEntities = *(m_ruleNamespaceDeclaration | functionGrammar.m_ruleFunctionDeclaration | functionGrammar);
			m_rulevMetaEntities.name("metaEntities");

			m_ruleModule		=	m_rulevMetaEntities.alias();
			m_ruleModule.name("module");

			m_ruleNamespaceDeclaration = 
					lexer(lexer::tokens::ETokenIDs::namespace_) 
				>	lexer(lexer::tokens::ETokenIDs::colon)
				>	identifierGrammar
				>	lexer(lexer::tokens::ETokenIDs::opening_brace)
				>	m_rulevMetaEntities
				>	lexer(lexer::tokens::ETokenIDs::closing_brace)
				;
			m_ruleNamespaceDeclaration.name("namespaceDeclaration");

#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(m_rulevMetaEntities)
				(m_ruleModule)
				(m_ruleNamespaceDeclaration)
			);
#endif
			// Error handling: on error in start, call the error handler.
			qi::on_error<qi::fail>(m_rulevMetaEntities,				TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleModule,					TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleNamespaceDeclaration,		TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotaion.
			//qi::on_success(m_rulevMetaEntities,				TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(m_ruleModule,						TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(m_ruleNamespaceDeclaration,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
		}
	}
}