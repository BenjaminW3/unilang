#pragma once

#include "ast_base.hpp"
#include "identifier_ast.hpp"
#include "expression_ast.hpp"
#include "statement_ast.hpp"
#include "function_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>

#include <ostream>
#include <list>

namespace unilang 
{
	//-----------------------------------------------------------------------------
	//! The namespace defining all the abstract syntax tree objects.
	//-----------------------------------------------------------------------------
	namespace ast
	{
		//#########################################################################
		//! A meta_entity can be multiple things.
		//#########################################################################
		struct meta_entity :	public ast_base,
								boost::spirit::extended_variant<	function_declaration, 
																	function_definition
																>
		{
			meta_entity();
			meta_entity(function_declaration const & val);
			meta_entity(function_definition const & val);
		};
		std::ostream& operator<<(std::ostream& out, meta_entity const& x);
		
		//#########################################################################
		//! A module is the root of the abstract syntax tree.
		//#########################################################################
		struct module : public ast_base
		{
			std::list<meta_entity> metaEntities;
		};
		std::ostream& operator<<(std::ostream& out, module const& x);
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::module,
    (std::list<unilang::ast::meta_entity>, metaEntities)
)