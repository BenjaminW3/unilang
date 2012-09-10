#include "function_ast.hpp"

namespace unilang 
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool function_declaration::isPure() const	
		{
			return false;
		}
		std::ostream& operator<<(std::ostream& out, function_declaration const& x)
		{
			out << "?";
			out << x.idf << ":";
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
			return out;
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool function_definition::isPure() const	
		{
			if(unpureQualifier){return false;}
			for(ast::variable_definition const & ret : return_value_definitions)
			{
				if(!ret.isPure()) {return false;}
			}
			for(ast::variable_declaration const & arg : parameter_declarations)
			{
				if(!arg.isPure()) {return false;}
			}
			return body.isPure();
		}
		std::ostream& operator<<(std::ostream& out, function_definition const& x)
		{
			out << x.idf << ":";
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
			out << '{' << std::endl;
			out << x.body << std::endl;
			out << '}' << std::endl;
			return out;
		}
	}
}