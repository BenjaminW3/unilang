#pragma once

#include <unilang/compiler/ast/function_ast.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_declaration,
	(unilang::ast::identifier, _idfName)
	(bool, _bHasUnpureQualifier)
	(std::vector<unilang::ast::type_declaration>, _vParameterTypes)
	(std::vector<unilang::ast::type_declaration>, _vReturnTypes)
)

BOOST_FUSION_ADAPT_STRUCT(
	unilang::ast::function_definition,
	(unilang::ast::identifier, _idfName)
	(bool, _bHasUnpureQualifier)
	(std::vector<unilang::ast::variable_declaration>, _vParameterDeclarations)
	(std::vector<unilang::ast::variable_definition>, _vReturnValueDefinitions)
	(unilang::ast::statement_vector, _body)
)