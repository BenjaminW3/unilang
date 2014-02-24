#include <unilang/compiler/ast/identifier_ast.hpp>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		identifier::identifier() :
			m_sName(/*"unnamed-identifier"*/)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		identifier::identifier(std::string const & name) :
			m_sName(name)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, identifier const & x)
		{
			out << x.m_sName; return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		namespaced_identifier::namespaced_identifier() :
			m_bGlobalNamespace	(false),
			m_sNames			()
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string namespaced_identifier::getMangledName() const
		{
			std::string sMangledName;
			if(m_bGlobalNamespace)	// the mangled name is always without starting double colon
			{
				sMangledName += "::";
			}
			bool bFirst (true);
			for(auto const & sName : m_sNames)
			{
				if(bFirst){bFirst = false;}
				else{sMangledName += "::";}
				sMangledName += sName;
			}
			return sMangledName;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, namespaced_identifier const & x)
		{
			out << /*(x.m_bGlobalNamespace ? "::" : "") +*/ x.getMangledName(); return out;
		}
	}
}