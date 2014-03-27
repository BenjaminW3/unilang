#pragma once

#include <unilang/compiler/ast/Statement.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SIfStatement,
	(unilang::ast::SExpression, m_expCondition)
	(unilang::ast::SStatementVector, _thenStatementList)
	(boost::optional<unilang::ast::SStatementVector>, _elseOptionalStatementList)
)

/*BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SWhileStatement,
	(unilang::ast::SExpression, m_expCondition)
	(unilang::ast::SStatementVector, m_body)
)*/

/*BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::SReturnStatement,
	(boost::optional<unilang::ast::SExpression>, _expr)
)*/