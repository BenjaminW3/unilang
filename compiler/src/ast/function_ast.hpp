#pragma once

#include "ast_base.hpp"
#include "identifier_ast.hpp"
#include "statement_ast.hpp"

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
			identifier idf;
			bool unpureQualifier;
			std::vector<type_declaration> parameter_types;
			std::vector<type_declaration> return_types;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_declaration const& x);

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function_definition :	public ast_base
		{
			identifier idf;
			bool unpureQualifier;
			std::vector<variable_declaration> parameter_declarations;
			std::vector<variable_definition> return_value_definitions;
			statement_list body;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_definition const& x);
	}
}