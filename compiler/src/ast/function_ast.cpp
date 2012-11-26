#include "function_ast.hpp"

#include <sstream>

namespace unilang 
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		// generate unique names for functions with different parameters. add:(i32,i32)
		//-------------------------------------------------------------------------
		std::string function_declaration::build_mangled_name() const
		{
			std::stringstream sstr;
			sstr << _identifier << ":";
			sstr << "(";
			bool bFirstArg = true;
			for(type_declaration const & decl : _parameter_types)
			{
				if(bFirstArg){bFirstArg = false;}
				else{sstr << ", ";}

				//sstr << decl;	// do not add mutable qualifiers for parameters
				sstr << decl._identifier;
			}
			sstr << ")";
			return sstr.str();
		}
		bool function_declaration::isPure() const	
		{
			return false;
		}
		std::ostream& operator<<(std::ostream& out, function_declaration const& x)
		{
			out << "?";
			out << x._identifier << ":";
			if(x._bHasUnpureQualifier)
			{
				out << " ~";
			}
			out << "(";
			bool bFirstArg = true;
			for(type_declaration const & decl : x._parameter_types)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(type_declaration const & decl : x._return_types)
			{
				if(bFirstRet){bFirstRet = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ")";
			return out;
		}
		
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool function_definition::isPure() const	
		{
			if(_bHasUnpureQualifier){return false;}
			for(ast::variable_definition const & ret : _return_value_definitions)
			{
				if(!ret.isPure()) {return false;}
			}
			for(ast::variable_declaration const & arg : _parameter_declarations)
			{
				if(!arg.isPure()) {return false;}
			}
			return _body.isPure();
		}
		std::ostream& operator<<(std::ostream& out, function_definition const& x)
		{
			out << x._identifier << ":";
			if(x._bHasUnpureQualifier)
			{
				out << "~";
			}
			out << "(";
			bool bFirstArg = true;
			for(variable_declaration const & def : x._parameter_declarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << def;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(variable_definition const & def : x._return_value_definitions)
			{
				if(bFirstRet){bFirstArg = false;}
				else{out << ", ";}

				out << def;
			}
			out << ")";
			out << '{' << std::endl;
			out << x._body << std::endl;
			out << '}' << std::endl;
			return out;
		}
	}
}