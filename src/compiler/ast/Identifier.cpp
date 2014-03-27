#include <unilang/compiler/ast/Identifier.hpp>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SIdentifier::SIdentifier() :
			m_sName(/*"unnamed-identifier"*/)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SIdentifier::SIdentifier(std::string const & name) :
			m_sName(name)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SIdentifier const & x)
		{
			out << x.m_sName; return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SNamespacedIdentifier::SNamespacedIdentifier() :
			m_bGlobalNamespace	(false),
			m_sNames			()
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string SNamespacedIdentifier::getMangledName() const
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
		std::ostream& operator<<(std::ostream& out, SNamespacedIdentifier const & x)
		{
			out << /*(x.m_bGlobalNamespace ? "::" : "") +*/ x.getMangledName(); return out;
		}
	}
}