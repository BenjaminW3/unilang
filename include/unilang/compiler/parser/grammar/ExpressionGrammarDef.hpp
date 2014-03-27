#pragma once

#include <unilang/compiler/parser/grammar/ExpressionGrammarFwd.hpp>

#include <unilang/compiler/parser/grammar/IdentifierGrammarFwd.hpp>

#include <unilang/compiler/lexer/Literals.hpp>
#include <unilang/compiler/lexer/LexerFwd.hpp>
#include <unilang/compiler/lexer/TokenIDsDef.hpp>
#include <unilang/compiler/parser/SourceErrorHandler.hpp>
#include <unilang/compiler/parser/AnnotationHandler.hpp>

#include <unilang/compiler/ast/OperatorsDef.hpp>
#include <unilang/compiler/ast/Expression.hpp>
#include <unilang/compiler/ast/fusion_adapt/Expression.hpp>

#include <unilang/compiler/parser/Spirit.hpp>

#include <boost/spirit/include/lex_plain_token.hpp>

namespace unilang
{
	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//-------------------------------------------------------------------------
		//! Constructor.
		//-------------------------------------------------------------------------
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		SExpressionGrammar<TSourceCodeIterator,TLexerTokenIterator>::SExpressionGrammar(
			SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler,
			SIdentifierGrammar<TSourceCodeIterator, TLexerTokenIterator> const & identifierGrammar, 
			lexer::CTokenLexer<TSourceCodeIterator> const & lexer) : 
			SExpressionGrammar::base_type(m_ruleExpression, "SExpressionGrammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			qi::tokenid_mask_type tokenid_mask;
			qi::matches_type matches;

			using boost::phoenix::function;

			typedef function<unilang::SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator> > TErrorHandlerFunction;
			typedef function<unilang::SAnnotationHandler<TLexerTokenIterator> > TAnnotationFunction;

			///////////////////////////////////////////////////////////////////////
			// Main expression grammar
			m_ruleExpression =
					m_ruleUnaryExpression
#ifdef TOKEN_ID
				>>  *(tokenid_mask(static_cast<operators::EOperators>(operators::EOperatorTypes::binaryOperation)) > m_ruleUnaryExpression)
#else
				>>  *(tokenid_mask(static_cast<size_t>(operators::EOperatorTypes::binaryOperation)) > m_ruleUnaryExpression)
#endif
				;
			m_ruleExpression.name("expression");

			m_ruleUnaryExpression =
					m_rulePostfixExpression
#ifdef TOKEN_ID
				|   (tokenid_mask(static_cast<operators::EOperators>(operators::EOperatorTypes::unaryOperation)) > m_ruleUnaryExpression)
#else
				|   (tokenid_mask(static_cast<size_t>(operators::EOperatorTypes::unaryOperation)) > m_ruleUnaryExpression)
#endif
				;
			m_ruleUnaryExpression.name("unaryExpression");

			m_rulePostfixExpression =
					m_ruleFunctionCall
				|	m_ruleVariableDefinition
				//|	m_ruleAssignmentStatement	// Assignment is no expression anymore.
				|   m_rulePrimaryExpression
				;
			m_rulePostfixExpression.name("postfix_expr");

			m_rulePrimaryExpression =
					lexer.m_tokLiteralString
				|	lexer.m_tokLiteralHexadecimal
				|	lexer.m_tokLiteralOctal
				|	lexer.m_tokLiteralBinary
				|	lexer.m_tokLiteralUnsignedFloat
				|	lexer.m_tokLiteralUnsignedInt
				|   lexer.m_tokLiteralBoolean
				|	identifierGrammar	// TODO: support .m_ruleNamespacedIdentifier
				|	(lexer(lexer::tokens::ETokenIDs::opening_parenthesis) > m_ruleExpression > lexer(lexer::tokens::ETokenIDs::closing_parenthesis))
				;
			m_rulePrimaryExpression.name("primaryExpression");

			/*m_ruleLValueExpression = 
					identifierGrammar
				//|	m_ruleVariableDefinition	// dont! just initialize it directly!
				|	m_ruleFunctionCall
				|   (lexer(lexer::tokens::ETokenIDs::opening_parenthesis) > m_ruleLValueExpression > lexer(lexer::tokens::ETokenIDs::closing_parenthesis))
				;
			m_ruleLValueExpression.name("lValueExpression");*/

			m_ruleMutableQualifier = 
					matches[lexer(lexer::tokens::ETokenIDs::compl)]
				;
			m_ruleMutableQualifier.name("mutableQualifier");

			m_ruleArgumentList = -(m_ruleExpression % lexer(lexer::tokens::ETokenIDs::comma));
			m_ruleArgumentList.name("argumentList");

			m_ruleFunctionCall =
					identifierGrammar.m_ruleNamespacedIdentifier
				>>	(m_ruleMutableQualifier
				>>	(lexer(lexer::tokens::ETokenIDs::opening_parenthesis)
				>	m_ruleArgumentList
				>   lexer(lexer::tokens::ETokenIDs::closing_parenthesis)))
				;
			// Paranthesis are required because of operator precedence between > and >> and how fusion_adapt exposes the attributes.
			// Without paranthesis the result would be tuple<tuple<IDF, bool>, Args>. So tuple<IDF, bool> is assigned to the identifier and Args would be assigned to bool -> Error. The right layout is tuple<IDF, bool, Args>.
			// See http://boost.2283326.n4.nabble.com/fusion-adapted-struct-compile-error-due-to-evil-expectation-operator-why-minimal-example-provided-td2683017.html
			m_ruleFunctionCall.name("functionCall");

			m_ruleTypeDeclaration =
					m_ruleMutableQualifier
				>>	identifierGrammar.m_ruleIdentifier	// TODO: support .m_ruleNamespacedIdentifier
				;
			m_ruleTypeDeclaration.name("typeDeclaration");

			// A Variable Declaration is of the form [<identifier>:][<type-qualifier>]<type>
			// <type-qualifier> can be '~'
			// <type> one of the predefined or user define types
			// <identifier> can be: -empty
			//						-a user defined identifier
			m_ruleVariableDeclaration =
				-(
					identifierGrammar.m_ruleIdentifier
					>>	lexer(lexer::tokens::ETokenIDs::colon)
				)
				>>	m_ruleTypeDeclaration
				;
			m_ruleVariableDeclaration.name("variableDeclaration");

			m_ruleDefinitionParameterList = 
					lexer(lexer::tokens::ETokenIDs::opening_brace)
				>	-(	m_ruleExpression % lexer(lexer::tokens::ETokenIDs::comma))
				>	lexer(lexer::tokens::ETokenIDs::closing_brace)
				;
			m_ruleDefinitionParameterList.name("definitionParameterList");

			// A Variable Definition is of the form [<identifier>:][<type-qualifier>]<type><parameterList>
			// <type-qualifier> can be '~'
			// <type> one of the predefined or user define types
			// <identifier> can be: -empty
			//						-a user defined identifier
			// <parameterList> can be:	-'{}' 
			//							-a list of expressions seperated by ',' surrounded by '{','}'
			m_ruleVariableDefinition =
					m_ruleVariableDeclaration
				>>	m_ruleDefinitionParameterList
				;
			m_ruleVariableDefinition.name("variableDefinition");

			m_ruleAssignmentExpression =
					identifierGrammar
#ifdef TOKEN_ID
				>>	tokenid_mask(static_cast<operators::EOperators>(operators::EOperatorTypes::assignmentOperation))
#else
				>>	tokenid_mask(static_cast<size_t>(operators::EOperatorTypes::assignmentOperation))
#endif
				>>	m_ruleExpression
				;
			m_ruleAssignmentExpression.name("assignmentExpression");

			///////////////////////////////////////////////////////////////////////
			// Debugging and error handling and reporting support.
#ifdef _DEBUG
			BOOST_SPIRIT_DEBUG_NODES(
				(m_ruleExpression)
				(m_ruleUnaryExpression)
				(m_rulePostfixExpression)
				(m_rulePrimaryExpression)
				(m_ruleFunctionCall)
				(m_ruleArgumentList)
				(m_ruleMutableQualifier)
				(m_ruleTypeDeclaration)
				(m_ruleVariableDeclaration)
				(m_ruleDefinitionParameterList)
				(m_ruleVariableDefinition)
				(m_ruleAssignmentExpression)
			);
#endif
			///////////////////////////////////////////////////////////////////////
			// Error handling: on error in expr, call the error handler.
			qi::on_error<qi::fail>(	m_ruleExpression,				TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleUnaryExpression,			TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_rulePostfixExpression,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_rulePrimaryExpression,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleFunctionCall,				TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleArgumentList,				TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleMutableQualifier,			TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleTypeDeclaration,			TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleVariableDeclaration,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleDefinitionParameterList,	TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleVariableDefinition,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>( m_ruleAssignmentExpression,		TErrorHandlerFunction(sourceErrorHandler)("Error! Expecting ", _4, _3));

			///////////////////////////////////////////////////////////////////////
			// Annotation: on success, call annotaion.
			qi::on_success(	m_ruleExpression,					TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(	m_ruleUnaryExpression,				TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(	m_rulePostfixExpression,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(	m_rulePrimaryExpression,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(	m_ruleFunctionCall,					TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			//qi::on_success(	m_ruleArgumentList,				TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));	// no success, this is no ast element
			//qi::on_success(	m_ruleMutableQualifier,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(	m_ruleTypeDeclaration,				TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(	m_ruleVariableDeclaration,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			//qi::on_success(	m_ruleDefinitionParameterList,	TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(	m_ruleVariableDefinition,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success( m_ruleAssignmentExpression,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
		}
	}
}