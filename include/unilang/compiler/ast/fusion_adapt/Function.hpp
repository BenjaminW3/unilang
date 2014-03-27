#pragma once

#include <unilang/compiler/ast/Function.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SFunctionDeclaration,
	(unilang::ast::SIdentifier, m_idfName)
	(bool, m_bHasUnpureQualifier)
	(std::vector<unilang::ast::SVariableDeclaration>, m_vParameterDeclarations)
	(std::vector<unilang::ast::SVariableDeclaration>, m_vReturnDeclarations)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SFunctionDefinition,
	(unilang::ast::SIdentifier, m_idfName)
	(bool, m_bHasUnpureQualifier)
	(std::vector<unilang::ast::SVariableDeclaration>, m_vParameterDeclarations)
	(unilang::ast::SStatementVector, m_body)
	(std::vector<unilang::ast::SExpression>, m_vReturnExpressions)
)