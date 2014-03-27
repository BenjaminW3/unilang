#pragma once

#include <unilang/compiler/parser/grammar/FunctionGrammarFwd.hpp>

#include <unilang/compiler/parser/grammar/IdentifierGrammarFwd.hpp>
#include <unilang/compiler/parser/grammar/ExpressionGrammarFwd.hpp>
#include <unilang/compiler/parser/grammar/StatementGrammarFwd.hpp>

#include <unilang/compiler/ast/Function.hpp>
#include <unilang/compiler/ast/fusion_adapt/Function.hpp>

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
		FunctionGrammar<TSourceCodeIterator,TLexerTokenIterator>::FunctionGrammar(
			SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
			SIdentifierGrammar<TSourceCodeIterator, TLexerTokenIterator> const & identifierGrammar, 
			SExpressionGrammar<TSourceCodeIterator, TLexerTokenIterator> const & expressionGrammar, 
			SStatementGrammar<TSourceCodeIterator, TLexerTokenIterator> const & statementGrammar, 
			lexer::CTokenLexer<TSourceCodeIterator> const & lexer) :
			FunctionGrammar::base_type(m_ruleFunctionDefinition, "FunctionGrammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			using boost::phoenix::function;

			typedef function<unilang::SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator> > TErrorHandlerFunction;
			typedef function<unilang::SAnnotationHandler<TLexerTokenIterator> > TAnnotationFunction;

			m_ruleVariableDeclarationList =
					lexer(lexer::tokens::ETokenIDs::opening_parenthesis)
				>	-(expressionGrammar.m_ruleVariableDeclaration % lexer(lexer::tokens::ETokenIDs::comma))
				>	lexer(lexer::tokens::ETokenIDs::closing_parenthesis)
				;
			m_ruleVariableDeclarationList.name("variableDeclarationList");

			m_ruleFunctionDeclaration =
					lexer(lexer::tokens::ETokenIDs::question_mark)
				>	identifierGrammar
				>	lexer(lexer::tokens::ETokenIDs::colon)
				>	expressionGrammar.m_ruleMutableQualifier
				>	m_ruleVariableDeclarationList
				>	lexer(lexer::tokens::ETokenIDs::arrow)
				>	m_ruleVariableDeclarationList
				;
			m_ruleFunctionDeclaration.name("functionDeclaration");

			m_ruleReturnDefinitionList =
				lexer(lexer::tokens::ETokenIDs::opening_parenthesis)
				>	-(expressionGrammar.m_ruleExpression % lexer(lexer::tokens::ETokenIDs::comma))
				>	lexer(lexer::tokens::ETokenIDs::closing_parenthesis)
				;
			m_ruleReturnDefinitionList.name("returnDefinitionList");

			m_ruleFunctionDefinition =
					identifierGrammar
				>	lexer(lexer::tokens::ETokenIDs::colon)
				>	expressionGrammar.m_ruleMutableQualifier
				>	m_ruleVariableDeclarationList
				>	-statementGrammar.m_ruleCompoundStatement
				>	lexer(lexer::tokens::ETokenIDs::arrow)
				>	m_ruleReturnDefinitionList
				;
			m_ruleFunctionDefinition.name("functionDefinition");
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(m_ruleVariableDeclarationList)
				(m_ruleFunctionDeclaration)
				(m_ruleReturnDefinitionList)
				(m_ruleFunctionDefinition)
			);
#endif
			// Error handling: on error in start, call the error handler.
			qi::on_error<qi::fail>(m_ruleVariableDeclarationList,	TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleFunctionDeclaration,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleReturnDefinitionList,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleFunctionDefinition,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));

			// Annotation: on success, call annotaion.
			//qi::on_success(m_ruleVariableDeclarationList,		TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(m_ruleFunctionDeclaration,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			//qi::on_success(m_ruleReturnDefinitionList,		TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(m_ruleFunctionDefinition,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
		}
	}
}