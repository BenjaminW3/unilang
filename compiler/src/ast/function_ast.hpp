#pragma once

#include "identifier_ast.hpp"
#include "statement_ast.hpp"

#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
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
			std::vector<type_declaration> argument_types;
			std::vector<type_declaration> return_types;
			identifier idf;
			bool pureQualifier;
		};
		inline std::ostream& operator<<(std::ostream& out, function_declaration const& x)
		{
			out << "? (";
			bool bFirstArg = true;
			for(type_declaration const & decl : x.argument_types)
			{
				if(!bFirstArg)
				{
					out << ", ";
					bFirstArg = false;
				}
				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(type_declaration const & decl : x.return_types)
			{
				if(!bFirstRet)
				{
					out << ", ";
					bFirstRet = false;
				}
				out << decl;
			}
			out << ") : " << x.idf;
			if(x.pureQualifier)
			{
				out << " =";
			}
			return out;
		}

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function_definition
		{
			std::vector<variable_definition> argument_definitions;
			std::vector<variable_definition> return_value_definitions;
			identifier idf;
			bool pureQualifier;
			statement_list body;
		};
		inline std::ostream& operator<<(std::ostream& out, function_definition const& x)
		{
			out << " (";
			bool bFirstArg = true;
			for(variable_definition const & def : x.argument_definitions)
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
			for(variable_definition const & def : x.return_value_definitions)
			{
				if(!bFirstRet)
				{
					out << ", ";
					bFirstRet = false;
				}
				out << def;
			}
			out << ") : " << x.idf;
			if(x.pureQualifier)
			{
				out << " =";
			}
			out << '{' << std::endl;
			out << x.body << std::endl;
			out << '}' << std::endl;
			return out;
		}
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_declaration,
    (std::vector<unilang::ast::type_declaration>, argument_types)
    (std::vector<unilang::ast::type_declaration>, return_types)
    (unilang::ast::identifier, idf)
	(bool, pureQualifier)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_definition,
    (std::vector<unilang::ast::variable_definition>, argument_definitions)
    (std::vector<unilang::ast::variable_definition>, return_value_definitions)
    (unilang::ast::identifier, idf)
	(bool, pureQualifier)
    (unilang::ast::statement_list, body)
)