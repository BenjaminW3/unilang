#pragma once

///////////////////////////////////////////////////////////////////////////////
// Spirit v2.5 allows you to suppress automatic generation
// of predefined terminals to speed up complation. With
// BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are
// responsible in creating instances of the terminals that
// you need (e.g. see qi::uint_type uint_ below).
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment this if you want to enable debugging
#define BOOST_SPIRIT_QI_DEBUG
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include "../../abstract_syntax_tree/abstract_syntax_tree.hpp"
#include "../error_handler.hpp"
#include "../skipper.hpp"
#include "../annotation.hpp"
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
			expression_grammar(error_handler<Iterator>& error_handler)
			  : expression_grammar::base_type(expr)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::char_type char_;
				qi::uint_type uint_;
				qi::_val_type _val;
				qi::raw_type raw;
				qi::lexeme_type lexeme;
				qi::alpha_type alpha;
				qi::alnum_type alnum;
				qi::bool_type bool_;

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
					;

				typenames.add
					("int")
					("float")
					("string")
					;

				keywords.add
					("true")
					("false")
					("if")
					("else")
					("while")
					//("int")
					("void")
					("return")
					;

				///////////////////////////////////////////////////////////////////////
				// Main expression grammar
				expr =
					logical_or_expr.alias()
					;
				expr.name("expr");

				logical_or_expr =
						logical_and_expr
					>> *(logical_or_op > logical_and_expr)
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
				unary_expr.name("unary_expr");

				primary_expr =
						uint_
					|   function_call
					|   identifier
					|   bool_
					|   '(' > expr > ')'
					;
				primary_expr.name("primary_expr");

				function_call =
						(identifier >> '(')
					>>   argument_list
					>>   ')'
					;
				function_call.name("function_call");

				argument_list = -(expr % ',');
				argument_list.name("argument_list");

				identifier =
						!keywords
					>>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
					;
				identifier.name("identifier");

				///////////////////////////////////////////////////////////////////////
				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(expr)
					(logical_or_expr)
					(logical_and_expr)
					(equality_expr)
					(relational_expr)
					(additive_expr)
					(multiplicative_expr)
					(unary_expr)
					(primary_expr)
					(function_call)
					(argument_list)
					(identifier)
				);

				///////////////////////////////////////////////////////////////////////
				// Error handling: on error in expr, call error_handler.
				on_error<fail>(	expr,
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

			qi::rule<Iterator, ast::expression(), skipper<Iterator> >
				expr, equality_expr, relational_expr,
				logical_or_expr, logical_and_expr,
				additive_expr, multiplicative_expr
				;

			qi::rule<Iterator, ast::operand(), skipper<Iterator> >
				unary_expr, primary_expr
				;

			qi::rule<Iterator, ast::function_call(), skipper<Iterator> >
				function_call
				;

			qi::rule<Iterator, std::list<ast::expression>(), skipper<Iterator> >
				argument_list
				;

			qi::rule<Iterator, ast::identifier(), skipper<Iterator> >
				identifier
				;

			qi::symbols<char, ast::optoken>
				logical_or_op, logical_and_op,
				equality_op, relational_op,
				additive_op, multiplicative_op, unary_op
				;

			qi::symbols<char>
				keywords, typenames
				;
		};
	}
}