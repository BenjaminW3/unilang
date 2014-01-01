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
		// forward declaration
		struct function_definition;

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
			function_declaration( identifier idfName, bool bHasUnpureQualifier, std::vector<type_declaration> const & vParameterTypes,
#ifdef IMPLEMENT_VAR_ARG
				bool bIsVarArg,
#endif
				std::vector<type_declaration> const & vReturnTypes);
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			function_declaration( function_definition const & functionDefinition );

			//-------------------------------------------------------------------------
			//! 
			//-------------------------------------------------------------------------
			bool isPure() const override;
			//-------------------------------------------------------------------------
			//! \return Unique names for functions with different parameters. add:(i32,i32)
			//-------------------------------------------------------------------------
			std::string build_mangled_name() const;

			identifier _idfName;
			bool _bHasUnpureQualifier;
			std::vector<type_declaration> _vParameterTypes;
#ifdef IMPLEMENT_VAR_ARG
			bool _bIsVarArg;
#endif
			std::vector<type_declaration> _vReturnTypes;
		};
		std::ostream& operator<<(std::ostream& out, function_declaration const& x);

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function_definition :
			public ast_base
		{
			//-------------------------------------------------------------------------
			//! 
			//-------------------------------------------------------------------------
			bool isPure() const override;

			identifier _idfName;
			bool _bHasUnpureQualifier;
			std::vector<variable_declaration> _vParameterDeclarations;
#ifdef IMPLEMENT_VAR_ARG
			bool _bIsVarArg;
#endif
			std::vector<variable_definition> _vReturnValueDefinitions;
			statement_vector _body;
		};
		std::ostream& operator<<(std::ostream& out, function_definition const& x);
	}
}