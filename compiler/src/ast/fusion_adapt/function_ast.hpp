#pragma once

#include "../function_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_declaration,
	(unilang::ast::identifier, _identifier)
	(bool, _bHasUnpureQualifier)
	(std::vector<unilang::ast::type_declaration>, _parameter_types)
	(std::vector<unilang::ast::type_declaration>, _return_types)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_definition,
	(unilang::ast::identifier, _identifier)
	(bool, _bHasUnpureQualifier)
	(std::vector<unilang::ast::variable_declaration>, _parameter_declarations)
	(std::vector<unilang::ast::variable_definition>, _return_value_definitions)
	(unilang::ast::statement_list, _body)
)