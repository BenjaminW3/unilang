#pragma once

#include "../function_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_declaration,
	(unilang::ast::identifier, idf)
	(bool, unpureQualifier)
	(std::vector<unilang::ast::type_declaration>, parameter_types)
	(std::vector<unilang::ast::type_declaration>, return_types)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_definition,
	(unilang::ast::identifier, idf)
	(bool, unpureQualifier)
	(std::vector<unilang::ast::variable_declaration>, parameter_declarations)
	(std::vector<unilang::ast::variable_definition>, return_value_definitions)
	(unilang::ast::statement_list, body)
)