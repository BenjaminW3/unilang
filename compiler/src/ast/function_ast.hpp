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
			bool unpureQualifier;
			std::vector<type_declaration> parameter_types;
			std::vector<type_declaration> return_types;
			identifier idf;
		};
		inline std::ostream& operator<<(std::ostream& out, function_declaration const& x)
		{
			out << "?";
			if(x.unpureQualifier)
			{
				out << " ~";
			}
			out << "(";
			bool bFirstArg = true;
			for(type_declaration const & decl : x.parameter_types)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(type_declaration const & decl : x.return_types)
			{
				if(bFirstRet){bFirstRet = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ")";
			out << ":" << x.idf;
			return out;
		}

		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct function_definition
		{
			bool unpureQualifier;
			std::vector<variable_declaration> parameter_declarations;
			std::vector<variable_definition> return_value_definitions;
			identifier idf;
			statement_list body;
		};
		inline std::ostream& operator<<(std::ostream& out, function_definition const& x)
		{
			if(x.unpureQualifier)
			{
				out << "~";
			}
			out << "(";
			bool bFirstArg = true;
			for(variable_declaration const & def : x.parameter_declarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << def;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(variable_definition const & def : x.return_value_definitions)
			{
				if(bFirstRet){bFirstArg = false;}
				else{out << ", ";}

				out << def;
			}
			out << ")";
			out << ":" << x.idf;
			out << '{' << std::endl;
			out << x.body << std::endl;
			out << '}' << std::endl;
			return out;
		}
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