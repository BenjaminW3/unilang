#pragma once

#include <unilang/compiler/ast/statement_ast.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::if_statement,
	(unilang::ast::expression, _expCondition)
	(unilang::ast::statement_vector, _thenStatementList)
	(boost::optional<unilang::ast::statement_vector>, _elseOptionalStatementList)
)

/*BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::while_statement,
	(unilang::ast::expression, _expCondition)
	(unilang::ast::statement_vector, _body)
)*/

/*BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::return_statement,
	(boost::optional<unilang::ast::expression>, _expr)
)*/