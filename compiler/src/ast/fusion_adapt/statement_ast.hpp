#pragma once

#include "../statement_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::if_statement,
	(unilang::ast::expression, condition)
	(unilang::ast::statement_list, then)
	(boost::optional<unilang::ast::statement_list>, else_)
)

/*BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::while_statement,
	(unilang::ast::expression, condition)
	(unilang::ast::statement_list, body)
)*/

/*BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::return_statement,
	(boost::optional<unilang::ast::expression>, expr)
)*/