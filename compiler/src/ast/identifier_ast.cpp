#include "identifier_ast.hpp"

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		identifier::identifier()
			:_name(/*"unnamed-identifier"*/)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		identifier::identifier(std::string const & name)
			:_name(name)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool identifier::isPure() const
		{
			// FIXME: using vars declared outside of a method is not pure! But this can also be used for types and functions.
			return true;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, identifier const& x)
		{
			out << x._name; return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		namespaced_identifier::namespaced_identifier()
			:m_bGlobalNamespace	(false),
			m_sNames			()
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool namespaced_identifier::isPure() const
		{
			// FIXME: using vars declared outside of a method is not pure! But this can also be used for types and functions.
			return true;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string namespaced_identifier::getMangledName() const
		{
			std::string sMangledName;
			if(m_bGlobalNamespace)	/*// the mangled name is always without starting double colon*/
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
		std::ostream& operator<<(std::ostream& out, namespaced_identifier const& x)
		{
			out << /*(x.m_bGlobalNamespace ? "::" : "") +*/ x.getMangledName(); return out;
		}
	}
}