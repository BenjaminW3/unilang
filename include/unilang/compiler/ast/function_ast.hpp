#pragma once

#include <unilang/compiler/ast/ast_base.hpp>
#include <unilang/compiler/ast/identifier_ast.hpp>
#include <unilang/compiler/ast/statement_ast.hpp>

#include <ostream>
#include <vector>

namespace unilang
{
	namespace ast
	{
		//#########################################################################
		//! A Function declaration.
		//#########################################################################
		struct function_declaration :
			public ast_base
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			function_declaration();
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			function_declaration( 
				identifier const & idfName, 
				bool bHasUnpureQualifier, 
				std::vector<variable_declaration> const & vParameterTypes,
				std::vector<variable_declaration> const & vReturnTypes);

			//-------------------------------------------------------------------------
			//! \return Unique names for functions with different parameters. add:(i32,i32)
			//-------------------------------------------------------------------------
			std::string build_mangled_name() const;

			identifier _idfName;
			bool _bHasUnpureQualifier;
			std::vector<variable_declaration> _vParameterDeclarations;
			std::vector<variable_declaration> _vReturnDeclarations;
		};
		std::ostream& operator<<(std::ostream& out, function_declaration const & x);

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function_definition :
			public ast_base
		{
			identifier _idfName;
			bool _bHasUnpureQualifier;
			std::vector<variable_declaration> _vParameterDeclarations;
			statement_vector _body;
			std::vector<expression> _vReturnExpressions;
		};
		std::ostream& operator<<(std::ostream& out, function_definition const & x);
	}
}