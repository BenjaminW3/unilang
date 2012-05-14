#pragma once

#include "identifier_ast.hpp"
#include "expression_ast.hpp"
#include "statement_ast.hpp"
#include "function_ast.hpp"

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <list>

namespace unilang 
{
	namespace ast
	{
		//#########################################################################
		//! structure/type/object definitions.
		//#########################################################################
		typedef boost::variant<
			function_declaration,
			function_definition
		> meta_entity;
		
		//#########################################################################
		//! A module.
		//#########################################################################
		struct module
		{
			std::list<meta_entity> metaEntities;
		};
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::module,
    (std::list<unilang::ast::meta_entity>, metaEntities)
)