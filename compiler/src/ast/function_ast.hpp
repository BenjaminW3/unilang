#pragma once

#include "identifier_ast.hpp"
#include "statement_ast.hpp"

#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/foreach.hpp>
#include <vector>

namespace unilang 
{
	namespace ast
	{
		//#########################################################################
		//! A Function declaration.
		//#########################################################################
		struct function_declaration
		{
			std::vector<variable_definition> arguments;
			std::vector<variable_definition> return_values;
			identifier idf;
			bool pureQualifier;
		};
		inline std::ostream& operator<<(std::ostream& out, function_declaration const& x)
		{
			out << "< (";
			bool bFirstArg = true;
			BOOST_FOREACH(variable_definition const & def , x.arguments)
			{
				if(!bFirstArg)
				{
					out << ", ";
					bFirstArg = false;
				}
				out << def;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			BOOST_FOREACH(variable_definition const & def , x.return_values)
			{
				if(!bFirstRet)
				{
					out << ", ";
					bFirstRet = false;
				}
				out << def;
			}
			out << ") > " << x.idf;
			if(x.pureQualifier)
			{
				out << " =";
			}
			return out;
		}

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function
		{
			function_declaration decl;
			statement_list body;
		};
		inline std::ostream& operator<<(std::ostream& out, function const& x)
		{
			out << x.decl << std::endl;
			out << '{' << std::endl;
			out << x.body << std::endl;
			out << '}' << std::endl;
			return out;
		}
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_declaration,
    (std::vector<unilang::ast::variable_definition>, arguments)
    (std::vector<unilang::ast::variable_definition>, return_values)
    (unilang::ast::identifier, idf)
	(bool, pureQualifier)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function,
    (unilang::ast::function_declaration, decl)
    (unilang::ast::statement_list, body)
)