#include "expression_grammar.hpp"

#include <boost/spirit/include/lex_plain_token.hpp>

#include "identifier_grammar.hpp"

#include "../../ast/operators_def.hpp"
#include "../../ast/fusion_adapt/expression_ast.hpp"

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
#ifdef TOKEN_ID
				>>  *(tokenid_mask(static_cast<operators::EOperators>(operators::EOperatorTypes::binaryOperation)) > unary_expr)
#else
				>>  *(tokenid_mask(static_cast<size_t>(operators::EOperatorTypes::binaryOperation)) > unary_expr)
#endif
				;
			expression.name("expression");

			unary_expr =
					postfix_expr
#ifdef TOKEN_ID
				|   (tokenid_mask(static_cast<operators::EOperators>(operators::EOperatorTypes::unaryOperation)) > unary_expr)
#else
				|   (tokenid_mask(static_cast<size_t>(operators::EOperatorTypes::unaryOperation)) > unary_expr)
#endif
				;
			unary_expr.name("unary_expr");

			postfix_expr =
					functionCall
				|	variableDefinition
				|	assignment_expr
				|   primary_expr
				;
			postfix_expr.name("postfix_expr");

			ufloat_expr = lexer._lit_ufloat;
			ufloat_expr.name("ufloat_expr");
			uint_expr = lexer._lit_uint;
			uint_expr.name("uint_expr");
			bool_expr = lexer._lit_boolean;
			bool_expr.name("bool_expr");

			primary_expr =
					ufloat_expr
				|	uint_expr
				|   bool_expr
				|   identifierGrammar
				|   (lexer("\\(") > expression > lexer("\\)"))
				;
			primary_expr.name("primary_expr");

			argumentList = -(expression % lexer(","));
			argumentList.name("argumentList");

			functionCall =
					identifierGrammar	
				>>	lexer("\\(")
				>	argumentList
				>   lexer("\\)")
				;
			functionCall.name("functionCall");

			mutableQualifier = 
					matches[lexer("~")]
				;
			mutableQualifier.name("mutableQualifier");

			typeDeclaration =
					mutableQualifier
				>>	identifierGrammar
				;
			typeDeclaration.name("typeDeclaration");

			// A Variable Declaration is of the form [<identifier>:][<type-qualifier>]<type>
			// <type-qualifier> can be '~'
			// <type> one of the predefined or user define types
			// <identifier> can be: -empty
			//						-a user defined identifier
			variableDeclaration =
				-(
					identifierGrammar
					>>	lexer(":")
				)
				>>	typeDeclaration
				;
			variableDeclaration.name("variableDeclaration");

			definitionParameterList = 
					lexer("\\{")
				>	-(	expression % lexer(","))
				>	lexer("\\}")
				;
			definitionParameterList.name("definitionParameterList");

			// A Variable Definition is of the form [<identifier>:][<type-qualifier>]<type><parameterList>
			// <type-qualifier> can be '~'
			// <type> one of the predefined or user define types
			// <identifier> can be: -empty
			//						-a user defined identifier
			// <parameterList> can be:	-'{}' 
			//							-a list of expressions seperated by ',' surrounded by '{','}'
			variableDefinition =
					variableDeclaration
				>>	definitionParameterList
				;
			variableDefinition.name("variableDefinition");

			assignment_expr =
					identifierGrammar
#ifdef TOKEN_ID
				>>	tokenid_mask(static_cast<operators::EOperators>(operators::EOperatorTypes::assignmentOperation))
#else
				>>	tokenid_mask(static_cast<size_t>(operators::EOperatorTypes::assignmentOperation))
#endif
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
				(ufloat_expr)
				(uint_expr)
				(bool_expr)
				(functionCall)
				(argumentList)
				(mutableQualifier)
				(typeDeclaration)
				(variableDeclaration)
				(definitionParameterList)
				(variableDefinition)
				(assignment_expr)
			);
#endif
			///////////////////////////////////////////////////////////////////////
			// Error handling: on error in expr, call error_handler.
			on_error<fail>(	expression,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	unary_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	postfix_expr,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	ufloat_expr,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	uint_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	bool_expr,			error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	primary_expr,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	functionCall,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	argumentList,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	mutableQualifier,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	typeDeclaration,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableDeclaration,error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	definitionParameterList,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>(	variableDefinition,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			on_error<fail>( assignment_expr,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));

			///////////////////////////////////////////////////////////////////////
			// Annotation: on success, call annotation.
			on_success(	expression,				annotation_function(error_handler.iters)(_val, _1));
			on_success(	unary_expr,				annotation_function(error_handler.iters)(_val, _1));
			on_success(	postfix_expr,			annotation_function(error_handler.iters)(_val, _1));
			//on_success(	ufloat_expr,				annotation_function(error_handler.iters)(_val, _1));
			//on_success(	uint_expr,				annotation_function(error_handler.iters)(_val, _1));
			//on_success(	bool_expr,				annotation_function(error_handler.iters)(_val, _1));
			on_success(	primary_expr,			annotation_function(error_handler.iters)(_val, _1));
			on_success(	functionCall,			annotation_function(error_handler.iters)(_val, _1));
			//on_success(	argumentList,			annotation_function(error_handler.iters)(_val, _1));
			//on_success(	mutableQualifier,		annotation_function(error_handler.iters)(_val, _1));
			on_success(	typeDeclaration,		annotation_function(error_handler.iters)(_val, _1));
			on_success(	variableDeclaration,	annotation_function(error_handler.iters)(_val, _1));
			on_success(	definitionParameterList,	annotation_function(error_handler.iters)(_val, _1));
			on_success(	variableDefinition,		annotation_function(error_handler.iters)(_val, _1));
			on_success( assignment_expr,		annotation_function(error_handler.iters)(_val, _1));
		}
	}
}