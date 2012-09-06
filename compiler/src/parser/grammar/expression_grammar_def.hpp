﻿#include "expression_grammar.hpp"

#include <boost/spirit/include/lex_plain_token.hpp>

#include "../../lexer/lexer.hpp"
#include "../error_handler.hpp"
#include "../annotation.hpp"

namespace unilang 
{
	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//-----------------------------------------------------------------------------
		//! Constructor
		//-----------------------------------------------------------------------------
		template <typename BaseIterator, typename Iterator>
		expression_grammar<BaseIterator,Iterator>::expression_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
																		identifier_grammar<BaseIterator, Iterator> const & identifierGrammar, 
																		lexer::token_lexer<BaseIterator>& lexer)
			: expression_grammar::base_type(expression, "expression_grammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			qi::tokenid_mask_type tokenid_mask;
			qi::matches_type matches;

			using qi::on_error;
			using qi::on_success;
			using qi::fail;
			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, Iterator> > error_handler_function;
			typedef function<unilang::annotation<Iterator> > annotation_function;

			///////////////////////////////////////////////////////////////////////
			// Main expression grammar
			expression =
					unary_expr
				>>  *(tokenid_mask(token_ids::op_binary) > unary_expr)
				;
			expression.name("expression");

			unary_expr =
					postfix_expr
				|   (tokenid_mask(token_ids::op_unary) > unary_expr)
				;
			unary_expr.name("unary_expr");

			postfix_expr =
					functionCall
				|   primary_expr
				;
			postfix_expr.name("postfix_expr");

			primary_expr =
					lexer.lit_float
				|	lexer.lit_uint
				|	lexer.lit_int
				|   lexer.lit_boolean
				|   identifierGrammar
				|	variableDefinition
				|   '(' > expression > ')'
				;
			primary_expr.name("primary_expr");

			argumentList = -(expression % ',');
			argumentList.name("argumentList");

			functionCall =
					(identifierGrammar	>> '(')
					>>   argumentList		// TODO >
					>>   ')'				// TODO >
				;
			functionCall.name("functionCall");

			mutableQualifier = 
				matches['~']
				;
			mutableQualifier.name("mutableQualifier");

			typeDeclaration =
					mutableQualifier
				>>	identifierGrammar
				;
			typeDeclaration.name("typeDeclaration");

			//boost::spirit::eps_type eps;
			variableIdentifier =
				-(
					/*	':'
					>>*/	identifierGrammar
					//>>	eps
				)
				;
			variableIdentifier.name("variableIdentifier");

			// A Variable Declaration is of the form [<type-qualifier>]<type>[:<identifier>]
			// <type-qualifier> can be '~'
			// <type> one of the predefined or user define types
			// <identifier> can be: -empty
			//						-a user defined identifier
			variableDeclaration =
					typeDeclaration
				>>	':'
				>>	variableIdentifier
				;
			variableDeclaration.name("variableDeclaration");

			parameterList = 
				'('
				>>	-( expression % ',')
				>>	')'
				;
			parameterList.name("parameterList");

			// A Variable Definition is of the form [<type-qualifier>]<type>[:<identifier>][<parameterList>]
			// <type-qualifier> can be '~'
			// <type> one of the predefined or user define types
			// <identifier> can be: -empty
			//						-a user defined identifier
			// <parameterList> can be:	-'()' 
			//							-a list of expressions seperated by ',' surrounded by '(',')'
			variableDefinition =
					variableDeclaration
				>>	parameterList
				;
			variableDefinition.name("variableDefinition");

			///////////////////////////////////////////////////////////////////////
			// Debugging and error handling and reporting support.
#ifdef _DEBUG
			BOOST_SPIRIT_DEBUG_NODES(
				(expression)
				(unary_expr)
				(postfix_expr)
				(primary_expr)
				(functionCall)
				(argumentList)
				(mutableQualifier)
				(typeDeclaration)
				(variableIdentifier)
				(variableDeclaration)
				(parameterList)
				(variableDefinition)
			);
#endif
			///////////////////////////////////////////////////////////////////////
			// Error handling: on error in expr, call error_handler.
			on_error<fail>(	expression,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	unary_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	postfix_expr,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	primary_expr,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	functionCall,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	argumentList,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	mutableQualifier,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	typeDeclaration,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableIdentifier,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableDeclaration,error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	parameterList,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableDefinition,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));

			///////////////////////////////////////////////////////////////////////
			// Annotation: on success, call annotation.
			//on_success(	unary_expr, annotation_function(error_handler.iters)(_val, _1));
			//on_success(	postfix_expr, annotation_function(error_handler.iters)(_val, _1));
			//on_success(	primary_expr, annotation_function(error_handler.iters)(_val, _1));
		}
	}
}