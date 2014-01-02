#include <unilang/compiler/ast/function_ast.hpp>

#include <sstream>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		function_declaration::function_declaration()
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		function_declaration::function_declaration(
			identifier const & idfName, 
			bool bHasUnpureQualifier, 
			std::vector<variable_type_declaration> const & vParameterTypes,
			std::vector<variable_type_declaration> const & vReturnTypes) :
			_idfName				(idfName),
			_bHasUnpureQualifier	(bHasUnpureQualifier),
			_vParameterTypes		(vParameterTypes),
			_vReturnTypes			(vReturnTypes)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		function_declaration::function_declaration( function_definition const & functionDefinition ) :
			_idfName				(functionDefinition._idfName),
			_bHasUnpureQualifier	(functionDefinition._bHasUnpureQualifier)
		{
			for(ast::variable_declaration const & decl : functionDefinition._vParameterDeclarations)
			{
				_vParameterTypes.push_back(decl._type);
			}
			for(ast::variable_definition const & def : functionDefinition._vReturnValueDefinitions)
			{
				_vReturnTypes.push_back(def._declaration._type);
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string function_declaration::build_mangled_name() const
		{
			std::stringstream sstr;
			sstr << _idfName << ":";
			if(_bHasUnpureQualifier)
			{
				sstr << "~";
			}
			sstr << "(";
			bool bFirstArg = true;
			for(variable_type_declaration const & decl : _vParameterTypes)
			{
				if(bFirstArg){bFirstArg = false;}
				else{sstr << ",";}

				sstr << decl.build_mangled_name();
			}
			sstr << ")";
			// The return value is not part of the mangled name because on the call site of a function the return type is irrelevant and is determined by overload resolution of the argument types.
			// So overloads on the return values only are not allowed.
			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, function_declaration const & x)
		{
			out << "?";
			out << x._idfName << ":";
			if(x._bHasUnpureQualifier)
			{
				out << " ~";
			}
			out << "(";
			bool bFirstArg = true;
			for(variable_type_declaration const & decl : x._vParameterTypes)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(variable_type_declaration const & decl : x._vReturnTypes)
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
		std::ostream& operator<<(std::ostream& out, function_definition const & x)
		{
			out << x._idfName << ":";
			if(x._bHasUnpureQualifier)
			{
				out << "~";
			}
			out << "(";
			bool bFirstArg = true;
			for(variable_declaration const & def : x._vParameterDeclarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << def;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(variable_definition const & def : x._vReturnValueDefinitions)
			{
				if(bFirstRet){bFirstRet = false;}
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