#pragma once

#include <unilang/compiler/ast/expression_ast.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_call,
	(unilang::ast::namespaced_identifier, _idfName)
	(std::vector<unilang::ast::expression>, _vArgumentExpressions)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::unary_expr,
	(unilang::operators::EOperators, _uiOperatorID)
	(unilang::ast::operand, _opOperand)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::unary_expr,
	(size_t, _uiOperatorID)
	(unilang::ast::operand, _opOperand)
)
#endif

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::operation,
	(unilang::operators::EOperators, _uiOperatorID)
	(unilang::ast::operand, _opOperand)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::operation,
	(size_t, _uiOperatorID)
	(unilang::ast::operand, _opOperand)
)
#endif

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::expression,
	(unilang::ast::operand, _firstOperand)
	(std::vector<unilang::ast::operation>, _vRestOperands)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::type_declaration,
	(bool, _bHasMutableQualifier)
	(unilang::ast::identifier, _idfName)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::variable_declaration,
	(boost::optional<unilang::ast::identifier>, _optionalIdentifier)
	(unilang::ast::type_declaration, _type)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::variable_definition,
	(unilang::ast::variable_declaration, _declaration)
	(std::vector<unilang::ast::expression>, _vParameterExpressions)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::assignment,
	(unilang::ast::identifier, _lhs)
	(unilang::operators::EOperators, _uiOperatorID)
	(unilang::ast::expression, _rhs)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::assignment,
	(unilang::ast::identifier, _lhs)
	(size_t, _uiOperatorID)
	(unilang::ast::expression, _rhs)
)
#endif