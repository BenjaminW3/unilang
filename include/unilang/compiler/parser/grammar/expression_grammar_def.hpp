#pragma once

#include <unilang/compiler/parser/grammar/expression_grammar.hpp>

#include <unilang/compiler/parser/grammar/identifier_grammar.hpp>

#include <unilang/compiler/lexer/lexer.hpp>
#include <unilang/compiler/parser/error_handler.hpp>
#include <unilang/compiler/parser/annotation.hpp>

#include <unilang/compiler/ast/operators_def.hpp>
#include <unilang/compiler/ast/expression_ast.hpp>
#include <unilang/compiler/ast/fusion_adapt/expression_ast.hpp>

#include <unilang/compiler/parser/spirit.hpp>

#include <boost/spirit/include/lex_plain_token.hpp>

namespace unilang
{
	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//-----------------------------------------------------------------------------
		//! Constructor.
		//-----------------------------------------------------------------------------
		template <typename BaseIterator, typename LexerIterator>
		expression_grammar<BaseIterator,LexerIterator>::expression_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
																			identifier_grammar<BaseIterator, LexerIterator> const & identifierGrammar, 
																			lexer::token_lexer<BaseIterator> const & lexer) : 
			expression_grammar::base_type(m_ruleExpression, "expression_grammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			qi::tokenid_mask_type tokenid_mask;
			qi::matches_type matches;

			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, LexerIterator> > error_handler_function;
			typedef function<unilang::annotation<LexerIterator> > annotation_function;

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
				//|	m_ruleAssignmentStatement
				|   m_rulePrimaryExpression
				;
			m_rulePostfixExpression.name("postfix_expr");

			m_rulePrimaryExpression =
					lexer.m_tokLiteralHexadecimal
				|	lexer.m_tokLiteralOctal
				|	lexer.m_tokLiteralBinary
				|	lexer.m_tokLiteralUnsignedFloat
				|	lexer.m_tokLiteralUnsignedInt
				|   lexer.m_tokLiteralBoolean
				|	identifierGrammar	// TODO: support .m_ruleNamespacedIdentifier
				|	(lexer("\\(") > m_ruleExpression > lexer("\\)"))
				;
			m_rulePrimaryExpression.name("primaryExpression");

			/*m_ruleLValueExpression = 
					identifierGrammar
				//|	m_ruleVariableDefinition	// dont! just initialize it directly!
				|	m_ruleFunctionCall
				|   (lexer("\\(") > m_ruleLValueExpression > lexer("\\)"))
				;
			m_ruleLValueExpression.name("lValueExpression");*/

			m_ruleMutableQualifier = 
					matches[lexer("~")]
				;
			m_ruleMutableQualifier.name("mutableQualifier");

			m_ruleArgumentList = -(m_ruleExpression % lexer(","));
			m_ruleArgumentList.name("argumentList");

			m_ruleFunctionCall =
					identifierGrammar.m_ruleNamespacedIdentifier
				>>	(m_ruleMutableQualifier
				>>	(lexer("\\(")
				>	m_ruleArgumentList
				>   lexer("\\)")))
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
					>>	lexer(":")
				)
				>>	m_ruleTypeDeclaration
				;
			m_ruleVariableDeclaration.name("variableDeclaration");

			m_ruleDefinitionParameterList = 
					lexer("\\{")
				>	-(	m_ruleExpression % lexer(","))
				>	lexer("\\}")
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
			// Error handling: on error in expr, call error_handler.
			qi::on_error<qi::fail>(	m_ruleExpression,				error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleUnaryExpression,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_rulePostfixExpression,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_rulePrimaryExpression,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleFunctionCall,				error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleArgumentList,				error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleMutableQualifier,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleTypeDeclaration,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleVariableDeclaration,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleDefinitionParameterList,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(	m_ruleVariableDefinition,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>( m_ruleAssignmentExpression,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));

			///////////////////////////////////////////////////////////////////////
			// Annotation: on success, call annotation.
			qi::on_success(	m_ruleExpression,					annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(	m_ruleUnaryExpression,				annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(	m_rulePostfixExpression,			annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(	m_rulePrimaryExpression,			annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(	m_ruleFunctionCall,					annotation_function(error_handler.iters)(_val, _1));
			//qi::on_success(	m_ruleArgumentList,				annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			//qi::on_success(	m_ruleMutableQualifier,			annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(	m_ruleTypeDeclaration,				annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(	m_ruleVariableDeclaration,			annotation_function(error_handler.iters)(_val, _1));
			//qi::on_success(	m_ruleDefinitionParameterList,	annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(	m_ruleVariableDefinition,			annotation_function(error_handler.iters)(_val, _1));
			qi::on_success( m_ruleAssignmentExpression,			annotation_function(error_handler.iters)(_val, _1));
		}
	}
}