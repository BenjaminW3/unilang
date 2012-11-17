#include "expression_grammar.hpp"

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
		//! Constructor.
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
					variableDefinition	// before function call for unnamed variable definitions
				|	functionCall
				|	assignment_expr
				|   primary_expr
				;
			postfix_expr.name("postfix_expr");

			float_expr = lexer.lit_float;
			float_expr.name("float_expr");
			uint_expr = lexer.lit_uint;
			uint_expr.name("uint_expr");
			int_expr = lexer.lit_int;
			int_expr.name("int_expr");
			bool_expr = lexer.lit_boolean;
			bool_expr.name("bool_expr");

			primary_expr =
					float_expr
				|	uint_expr
				|	int_expr
				|   bool_expr
				|   identifierGrammar
				|   (lexer("\\(") > expression > lexer("\\)"))
				;
			primary_expr.name("primary_expr");

			argumentList = -(expression % lexer(","));
			argumentList.name("argumentList");

			functionCall =
					(identifierGrammar	>> lexer("\\("))
				>>   argumentList
				>   lexer("\\)")
				;
			functionCall.name("functionCall");

			mutableQualifier = 
					matches[lexer("~")]
				;
			mutableQualifier.name("mutableQualifier");

			typeDeclaration =
					mutableQualifier
				>	identifierGrammar
				;
			typeDeclaration.name("typeDeclaration");

			variableIdentifier = 
				-(
					identifierGrammar
				)
				;
			variableIdentifier.name("variableIdentifier");

			// A Variable Declaration is of the form [<type-qualifier>]<type>[:<identifier>]
			// <type-qualifier> can be '~'
			// <type> one of the predefined or user define types
			// <identifier> can be: -empty
			//						-a user defined identifier
			variableDeclaration =
					variableIdentifier
				>>	lexer(":")
				>	typeDeclaration
				;
			variableDeclaration.name("variableDeclaration");

			parameterList = 
					lexer("\\(")
				>	-( expression % lexer(","))
				>	lexer("\\)")
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

			assignment_expr =
					identifierGrammar
				>>	tokenid_mask(token_ids::op_assign)
				>>	expression
				;
			assignment_expr.name("assignment_expr");

			///////////////////////////////////////////////////////////////////////
			// Debugging and error handling and reporting support.
#ifdef _DEBUG
			BOOST_SPIRIT_DEBUG_NODES(
				(expression)
				(unary_expr)
				(postfix_expr)
				(primary_expr)
				(float_expr)
				(uint_expr)
				(int_expr)
				(bool_expr)
				(functionCall)
				(argumentList)
				(mutableQualifier)
				(typeDeclaration)
				(variableIdentifier)
				(variableDeclaration)
				(parameterList)
				(variableDefinition)
				(assignment_expr)
			);
#endif
			///////////////////////////////////////////////////////////////////////
			// Error handling: on error in expr, call error_handler.
			on_error<fail>(	expression,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	unary_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	postfix_expr,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	float_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	uint_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	int_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	bool_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	primary_expr,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	functionCall,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	argumentList,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	mutableQualifier,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	typeDeclaration,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableIdentifier,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableDeclaration,error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	parameterList,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableDefinition,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>( assignment_expr,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));

			///////////////////////////////////////////////////////////////////////
			// Annotation: on success, call annotation.
			on_success(	expression,				annotation_function(error_handler.iters)(_val, _1));
			on_success(	unary_expr,				annotation_function(error_handler.iters)(_val, _1));
			on_success(	postfix_expr,			annotation_function(error_handler.iters)(_val, _1));
			//on_success(	float_expr,				annotation_function(error_handler.iters)(_val, _1));
			//on_success(	uint_expr,				annotation_function(error_handler.iters)(_val, _1));
			//on_success(	int_expr,				annotation_function(error_handler.iters)(_val, _1));
			//on_success(	bool_expr,				annotation_function(error_handler.iters)(_val, _1));
			on_success(	primary_expr,			annotation_function(error_handler.iters)(_val, _1));
			on_success(	functionCall,			annotation_function(error_handler.iters)(_val, _1));
			//on_success(	argumentList,			annotation_function(error_handler.iters)(_val, _1));
			//on_success(	mutableQualifier,		annotation_function(error_handler.iters)(_val, _1));
			on_success(	typeDeclaration,		annotation_function(error_handler.iters)(_val, _1));
			//on_success(	variableIdentifier,		annotation_function(error_handler.iters)(_val, _1));
			on_success(	variableDeclaration,	annotation_function(error_handler.iters)(_val, _1));
			on_success(	parameterList,			annotation_function(error_handler.iters)(_val, _1));
			on_success(	variableDefinition,		annotation_function(error_handler.iters)(_val, _1));
			on_success( assignment_expr,		annotation_function(error_handler.iters)(_val, _1));
		}
	}
}