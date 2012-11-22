#pragma once

#include "../expression_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_call,
	(unilang::ast::identifier, idf)
	(std::list<unilang::ast::expression>, arguments)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::unary_expr,
	(unilang::operators::EOperators, operator_)
	(unilang::ast::operand, operand_)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::unary_expr,
	(size_t, operator_)
	(unilang::ast::operand, operand_)
)
#endif

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::operation,
	(unilang::operators::EOperators, operator_)
	(unilang::ast::operand, operand_)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::operation,
	(size_t, operator_)
	(unilang::ast::operand, operand_)
)
#endif

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::expression,
	(unilang::ast::operand, first)
	(std::list<unilang::ast::operation>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::type_declaration,
	(bool, mutableQualifier)
	(unilang::ast::identifier, type_identifier)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::variable_declaration,
	(boost::optional<unilang::ast::identifier>, name)
	(unilang::ast::type_declaration, type)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::variable_definition,
	(unilang::ast::variable_declaration, decl)
	(std::list<unilang::ast::expression>, parameters)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::assignment,
	(unilang::ast::identifier, lhs)
	(unilang::operators::EOperators, operator_)
	(unilang::ast::expression, rhs)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::assignment,
	(unilang::ast::identifier, lhs)
	(size_t, operator_)
	(unilang::ast::expression, rhs)
)
#endif