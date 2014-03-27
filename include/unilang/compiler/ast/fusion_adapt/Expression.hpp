#pragma once

#include <unilang/compiler/ast/Expression.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SFunctionCall,
	(unilang::ast::SNamespacedIdentifier, m_idfName)
	(bool, m_bHasUnpureQualifier)
	(std::vector<unilang::ast::SExpression>, m_vArgumentExpressions)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SUnaryExpr,
	(unilang::operators::EOperators, m_uiOperatorID)
	(unilang::ast::SOperand, m_opOperand)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SUnaryExpr,
	(size_t, m_uiOperatorID)
	(unilang::ast::SOperand, m_opOperand)
)
#endif

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SOperation,
	(unilang::operators::EOperators, m_uiOperatorID)
	(unilang::ast::SOperand, m_opOperand)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SOperation,
	(size_t, m_uiOperatorID)
	(unilang::ast::SOperand, m_opOperand)
)
#endif

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SExpression,
	(unilang::ast::SOperand, m_firstOperand)
	(std::vector<unilang::ast::SOperation>, m_vRestOperands)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SVariableTypeDeclaration,
	(bool, m_bHasMutableQualifier)
	(unilang::ast::SIdentifier, m_idfName)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SVariableDeclaration,
	(boost::optional<unilang::ast::SIdentifier>, m_optionalIdentifier)
	(unilang::ast::SVariableTypeDeclaration, m_type)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SVariableDefinition,
	(unilang::ast::SVariableDeclaration, m_declaration)
	(std::vector<unilang::ast::SExpression>, m_vParameterExpressions)
)

#ifdef TOKEN_ID
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SAssignment,
	(unilang::ast::SIdentifier, m_lhs)
	(unilang::operators::EOperators, m_uiOperatorID)
	(unilang::ast::SExpression, m_rhs)
)
#else
BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SAssignment,
	(unilang::ast::SIdentifier, m_lhs)
	(size_t, m_uiOperatorID)
	(unilang::ast::SExpression, m_rhs)
)
#endif