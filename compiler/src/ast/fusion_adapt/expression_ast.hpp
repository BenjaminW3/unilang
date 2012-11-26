#pragma once

#include "../expression_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_call,
	(unilang::ast::identifier, _identifier)
	(std::list<unilang::ast::expression>, _lArgumentExpressions)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::unary_expr,
	(unilang::operators::EOperators, _operator)
	(unilang::ast::operand, _operand)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::unary_expr,
	(size_t, _operator)
	(unilang::ast::operand, _operand)
)
#endif

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::operation,
	(unilang::operators::EOperators, _operator)
	(unilang::ast::operand, _operand)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::operation,
	(size_t, _operator)
	(unilang::ast::operand, _operand)
)
#endif

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::expression,
	(unilang::ast::operand, _first)
	(std::list<unilang::ast::operation>, _rest)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::type_declaration,
	(bool, _bHasMutableQualifier)
	(unilang::ast::identifier, _identifier)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::variable_declaration,
	(boost::optional<unilang::ast::identifier>, _optionalIdentifier)
	(unilang::ast::type_declaration, _type)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::variable_definition,
	(unilang::ast::variable_declaration, _declaration)
	(std::list<unilang::ast::expression>, _lParameterExpressions)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::assignment,
	(unilang::ast::identifier, _lhs)
	(unilang::operators::EOperators, _operator)
	(unilang::ast::expression, _rhs)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::assignment,
	(unilang::ast::identifier, _lhs)
	(size_t, _operator)
	(unilang::ast::expression, _rhs)
)
#endif