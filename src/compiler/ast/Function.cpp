#include <unilang/compiler/ast/Function.hpp>

#include <unilang/compiler/ast/Expression.hpp>

#include <sstream>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SFunctionDeclaration::SFunctionDeclaration()
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SFunctionDeclaration::SFunctionDeclaration(
			SIdentifier const & idfName,
			bool bHasUnpureQualifier, 
			std::vector<SVariableDeclaration> const & vParameterTypes,
			std::vector<SVariableDeclaration> const & vReturnTypes) :
			m_idfName				(idfName),
			m_bHasUnpureQualifier	(bHasUnpureQualifier),
			m_vParameterDeclarations	(vParameterTypes),
			m_vReturnDeclarations	(vReturnTypes)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string SFunctionDeclaration::build_mangled_name() const
		{
			std::stringstream sstr;
			sstr << m_idfName << ":";
			if(m_bHasUnpureQualifier)
			{
				sstr << "~";
			}
			sstr << "(";
			bool bFirstArg = true;
			for(auto const & decl : m_vParameterDeclarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{sstr << ",";}

				sstr << decl.m_type.build_mangled_name();
			}
			sstr << ")";
			// The return value is not part of the mangled name because on the call site of a function the return type is irrelevant and is determined by overload resolution of the argument types.
			// So overloads on the return values only are not allowed.
			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SFunctionDeclaration const & x)
		{
			out << "?";
			out << x.m_idfName << ":";
			if(x.m_bHasUnpureQualifier)
			{
				out << " ~";
			}
			out << "(";
			bool bFirstArg = true;
			for(auto const & decl : x.m_vParameterDeclarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(auto const & decl : x.m_vReturnDeclarations)
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
		std::ostream& operator<<(std::ostream& out, SFunctionDefinition const & x)
		{
			out << x.m_idfName << ":";
			if(x.m_bHasUnpureQualifier)
			{
				out << "~";
			}
			out << "(";
			bool bFirstArg = true;
			for(auto const & decl : x.m_vParameterDeclarations)
			{
				if(bFirstArg){bFirstArg = false;}
				else{out << ", ";}

				out << decl;
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(auto const & exp : x.m_vReturnExpressions)
			{
				if(bFirstRet){bFirstRet = false;}
				else{out << ", ";}

				out << exp;
			}
			out << ")";
			out << '{' << std::endl;
			out << x.m_body << std::endl;
			out << '}' << std::endl;
			return out;
		}
	}
}