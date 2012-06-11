#pragma once

#include "identifier_grammar.hpp"

#include "../../ast/expression_ast.hpp"

#include "../error_handler.hpp"
#include "../skipper.hpp"
#include "../annotation.hpp"

#include "../spirit.hpp"

#include <vector>

#if defined(_MSC_VER)
//#pragma warning(disable: 4127)	// warning C4127: conditional expression is constant
//#pragma warning(disable: 4512)	// warning C4512: assignment operator could not be generated
#endif

namespace unilang 
{
	namespace parser
	{
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;

		///////////////////////////////////////////////////////////////////////////////
		//  The expression grammar
		///////////////////////////////////////////////////////////////////////////////
		template <typename Iterator>
		struct expression_grammar : qi::grammar<Iterator, ast::expression(), skipper<Iterator> >
		{
			expression_grammar(error_handler<Iterator>& error_handler, identifier_grammar<Iterator> const & identifierGrammar)
			  : expression_grammar::base_type(expression, "expression_grammar")
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::char_type char_;
				qi::uint_type uint_;
				qi::_val_type _val;
				qi::bool_type bool_;
				qi::matches_type matches;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				///////////////////////////////////////////////////////////////////////
				// Tokens
				logical_or_op.add
					("||", ast::op_or)
					;

				logical_and_op.add
					("&&", ast::op_and)
					;

				equality_op.add
					("==", ast::op_equal)
					("!=", ast::op_not_equal)
					;

				relational_op.add
					("<", ast::op_less)
					("<=", ast::op_less_equal)
					(">", ast::op_greater)
					(">=", ast::op_greater_equal)
					;

				additive_op.add
					("+", ast::op_plus)
					("-", ast::op_minus)
					;

				multiplicative_op.add
					("*", ast::op_times)
					("/", ast::op_divide)
					;

				unary_op.add
					("+", ast::op_positive)
					("-", ast::op_negative)
					("!", ast::op_not)
					("$", ast::op_stringify)
					;

				///////////////////////////////////////////////////////////////////////
				// Main expression grammar
				
				unaryOp_expr =
						(unary_op > expression)
					;
				unaryOp_expr.name("unaryOp_expr");

				// without paranthesis left-recursion leads to stack-overflow while parsing
				addOp_expr = 
					'('
					>>	expression
					>>	additive_op
					>>	expression
					>>	')'
					;
				addOp_expr.name("addOp_expr");

				binaryOp_expr =
					addOp_expr.alias()
					;
				binaryOp_expr.name("binaryOp_expr");
				
				expression = 
						uint_
					|   bool_
					|   functionCall
					|	identifierGrammar
					|	unaryOp_expr
					|	binaryOp_expr
					|	variableDefinition
					|	'(' > expression > ')'
					;
				expression.name("expression");
				
				/*logical_or_expr =
						logical_and_expr
					>>	*(	logical_or_op >	logical_and_expr)
					;
				logical_or_expr.name("logical_or_expr");

				logical_and_expr =
						equality_expr
					>> *(logical_and_op > equality_expr)
					;
				logical_and_expr.name("logical_and_expr");

				equality_expr =
						relational_expr
					>> *(equality_op > relational_expr)
					;
				equality_expr.name("equality_expr");

				relational_expr =
						additive_expr
					>> *(relational_op > additive_expr)
					;
				relational_expr.name("relational_expr");
				
				additive_expr =
						multiplicative_expr
					>> *(additive_op > multiplicative_expr)
					;
				additive_expr.name("additive_expr");

				multiplicative_expr =
						unary_expr
					>> *(multiplicative_op > unary_expr)
					;
				multiplicative_expr.name("multiplicative_expr");

				unary_expr =
						primary_expr
					|   (unary_op > unary_expr)
					;
				unary_expr.name("unary_expr");*/

				argumentList = -(expression % ',');
				argumentList.name("argumentList");

				functionCall =
						(identifierGrammar >> '(')
					>>   argumentList
					>>   ')'
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

				variableDefinitionIdentifier = 
					-identifierGrammar
					;
				variableDefinitionIdentifier.name("variableDefinitionIdentifier");
				
				parameterList = 
					-(
						'('
						>>	-( expression % ',')
						>>	')'
					)
					;
				parameterList.name("parameterList");

				//∈↦
				// A Variable Definition is of the form <type-qualifier><type><identifier><parameterList>
				// <type-qualifier> can be '~'
				// <type> one of the predefined or user define types
				// <identifier> can be: -empty
				//						-a user defined identifier name surrounded by '[',']'
				// <parameterList> can be:	-empty, 
				//							-'()' 
				//							-a list of expressions seperated by ',' surrounded by '(',')'
				variableDefinition =
						typeDeclaration
					>>	":"
					>>	variableDefinitionIdentifier
					>>	parameterList
					;
				variableDefinition.name("variableDefinition");

				///////////////////////////////////////////////////////////////////////
				// Debugging and error handling and reporting support.
#ifdef _DEBUG
				BOOST_SPIRIT_DEBUG_NODES(
					(unaryOp_expr)
					(binaryOp_expr)
					(addOp_expr)
					(expression)
					(logical_or_expr)
					(logical_and_expr)
					(equality_expr)
					(relational_expr)
					(additive_expr)
					(multiplicative_expr)
					(functionCall)
					(argumentList)
					(mutableQualifier)
					(typeDeclaration)
					(variableDefinitionIdentifier)
					(parameterList)
					(variableDefinition)
				);
#endif
				///////////////////////////////////////////////////////////////////////
				// Error handling: on error in expr, call error_handler.
				on_error<fail>(	expression,
								error_handler_function(error_handler)
								("Error! Expecting ", _4, _3));

#if defined(_MSC_VER)
#pragma warning(disable: 4512)	// warning C4512: assignment operator could not be generated
#endif
				///////////////////////////////////////////////////////////////////////
				// Annotation: on success in primary_expr, call annotation.
				/*on_success(	primary_expr, 
							annotation_function(error_handler.iters)
							(_val, _1));*/
#if defined(_MSC_VER)
#pragma warning(default: 4512)
#endif
			}
			
			qi::symbols<char, ast::optoken> logical_or_op, logical_and_op, equality_op, relational_op, additive_op, multiplicative_op, unary_op;

			qi::rule<Iterator, ast::unaryOp(), skipper<Iterator> >
				unaryOp_expr
				;

			qi::rule<Iterator, ast::binaryOp(), skipper<Iterator> >
				binaryOp_expr, addOp_expr
				;

			qi::rule<Iterator, ast::expression(), skipper<Iterator> > expression,
				equality_expr, relational_expr,
				logical_or_expr, logical_and_expr,
				additive_expr, multiplicative_expr,
				unary_expr, primary_expr
				;

			qi::rule<Iterator, std::list<ast::expression>(), skipper<Iterator> > argumentList;
			qi::rule<Iterator, ast::function_call(), skipper<Iterator> > functionCall;
			
			
			qi::rule<Iterator, bool(), skipper<Iterator> > mutableQualifier;
			qi::rule<Iterator, ast::type_declaration(), skipper<Iterator> > typeDeclaration;

			qi::rule<Iterator, boost::optional<ast::identifier>(), skipper<Iterator> > variableDefinitionIdentifier;
			qi::rule<Iterator, boost::optional<std::list<ast::expression>>(), skipper<Iterator> > parameterList;
			qi::rule<Iterator, ast::variable_definition(), skipper<Iterator> > variableDefinition;
		};
	}
}