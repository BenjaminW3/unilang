#pragma once

#include "ast_base.hpp"
#include "identifier_ast.hpp"
#include "statement_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

#include <ostream>
#include <vector>

namespace unilang 
{
	namespace ast
	{
		//#########################################################################
		//! A Function declaration.
		//#########################################################################
		struct function_declaration :	public ast_base
		{
			bool unpureQualifier;
			std::vector<type_declaration> parameter_types;
			std::vector<type_declaration> return_types;
			identifier idf;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_declaration const& x);

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function_definition :	public ast_base
		{
			bool unpureQualifier;
			std::vector<variable_declaration> parameter_declarations;
			std::vector<variable_definition> return_value_definitions;
			identifier idf;
			statement_list body;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_definition const& x);
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_declaration,
	(bool, unpureQualifier)
    (std::vector<unilang::ast::type_declaration>, parameter_types)
    (std::vector<unilang::ast::type_declaration>, return_types)
    (unilang::ast::identifier, idf)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_definition,
	(bool, unpureQualifier)
    (std::vector<unilang::ast::variable_declaration>, parameter_declarations)
    (std::vector<unilang::ast::variable_definition>, return_value_definitions)
    (unilang::ast::identifier, idf)
    (unilang::ast::statement_list, body)
)