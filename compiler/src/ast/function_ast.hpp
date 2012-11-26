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
			identifier _identifier;
			bool _bHasUnpureQualifier;
			std::vector<type_declaration> _parameter_types;
#ifdef IMPLEMENT_VAR_ARG
			bool _bIsVarArg;
#endif
			std::vector<type_declaration> _return_types;

			std::string build_mangled_name() const;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_declaration const& x);

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function_definition :	public ast_base
		{
			identifier _identifier;
			bool _bHasUnpureQualifier;
			std::vector<variable_declaration> _parameter_declarations;
#ifdef IMPLEMENT_VAR_ARG
			bool _bIsVarArg;
#endif
			std::vector<variable_definition> _return_value_definitions;
			statement_list _body;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_definition const& x);
	}
}