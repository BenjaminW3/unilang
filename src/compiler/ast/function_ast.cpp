#include <unilang/compiler/ast/function_ast.hpp>

#include <unilang/compiler/ast/expression_ast.hpp>

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
			std::vector<variable_declaration> const & vParameterTypes,
			std::vector<variable_declaration> const & vReturnTypes) :
			_idfName				(idfName),
			_bHasUnpureQualifier	(bHasUnpureQualifier),
			_vParameterDeclarations	(vParameterTypes),
			_vReturnDeclarations	(vReturnTypes)
		{
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
			for(auto const & decl : _vParameterDeclarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{sstr << ",";}

				sstr << decl._type.build_mangled_name();
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
			for(auto const & decl : x._vParameterDeclarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(auto const & decl : x._vReturnDeclarations)
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
			for(auto const & decl : x._vParameterDeclarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(auto const & exp : x._vReturnExpressions)
			{
				if(bFirstRet){bFirstRet = false;}
				else{out << ", ";}

				out << exp;
			}
			out << ")";
			out << '{' << std::endl;
			out << x._body << std::endl;
			out << '}' << std::endl;
			return out;
		}
	}
}