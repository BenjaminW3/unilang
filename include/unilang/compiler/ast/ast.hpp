#pragma once

#include <unilang/compiler/ast/ast_base.hpp>
#include <unilang/compiler/ast/identifier_ast.hpp>
#include <unilang/compiler/ast/expression_ast.hpp>
#include <unilang/compiler/ast/statement_ast.hpp>
#include <unilang/compiler/ast/function_ast.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>

#include <ostream>
#include <vector>

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining all the abstract syntax tree objects.
	//-------------------------------------------------------------------------
	namespace ast
	{
		// forward declarations
		struct namespace_declaration;
		//#########################################################################
		//! A meta_entity can be multiple things.
		//#########################################################################
		struct meta_entity :
			public ast_base,
			boost::spirit::extended_variant<	function_declaration, 
												function_definition, 
												boost::recursive_wrapper<namespace_declaration>
											>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			meta_entity();
			meta_entity(function_declaration const & val);
			meta_entity(function_definition const & val);
			meta_entity(namespace_declaration const & val);
		};
		std::ostream& operator<<(std::ostream& out, meta_entity const & x);
		std::ostream& operator<<(std::ostream& out, std::vector<meta_entity> const & x);

		//#########################################################################
		//! A namespace declaration.
		//#########################################################################
		struct namespace_declaration :
			public ast_base
		{
			identifier _idfName;
			std::vector<meta_entity> _metaEntities;
		};
		std::ostream& operator<<(std::ostream& out, namespace_declaration const & x);

		//#########################################################################
		//! A module is the root of the abstract syntax tree.
		//#########################################################################
		struct module :
			public ast_base
		{
			std::vector<meta_entity> _metaEntities;
		};
		std::ostream& operator<<(std::ostream& out, module const & x);
	}
}