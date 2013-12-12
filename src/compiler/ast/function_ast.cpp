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
		function_declaration::function_declaration( identifier idfName, bool bHasUnpureQualifier, std::vector<type_declaration> const & vParameterTypes,
#ifdef IMPLEMENT_VAR_ARG
			bool bIsVarArg,
#endif
			std::vector<type_declaration> const & vReturnTypes)
			:_idfName				(idfName),
			_bHasUnpureQualifier	(bHasUnpureQualifier),
			_vParameterTypes		(vParameterTypes),
#ifdef IMPLEMENT_VAR_ARG
			_bIsVarArg				(bIsVarArg),
#endif
			_vReturnTypes			(vReturnTypes)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		function_declaration::function_declaration( function_definition const & functionDefinition )
			:_idfName				(functionDefinition._idfName),
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
		bool function_declaration::isPure() const
		{
			return false;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string function_declaration::build_mangled_name() const
		{
			std::stringstream sstr;
			sstr << _idfName << ":";
			sstr << "(";
			bool bFirstArg = true;
			for(type_declaration const & decl : _vParameterTypes)
			{
				if(bFirstArg){bFirstArg = false;}
				else{sstr << ", ";}

				sstr << decl.build_mangled_name();
			}
			sstr << ")";
			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, function_declaration const& x)
		{
			out << "?";
			out << x._idfName << ":";
			if(x._bHasUnpureQualifier)
			{
				out << " ~";
			}
			out << "(";
			bool bFirstArg = true;
			for(type_declaration const & decl : x._vParameterTypes)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(type_declaration const & decl : x._vReturnTypes)
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
			for(ast::variable_definition const & ret : _vReturnValueDefinitions)
			{
				if(!ret.isPure()) {return false;}
			}
			for(ast::variable_declaration const & arg : _vParameterDeclarations)
			{
				if(!arg.isPure()) {return false;}
			}
			return _body.isPure();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, function_definition const& x)
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