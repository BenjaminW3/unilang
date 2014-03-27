#include <unilang/compiler/abt/TypeBTNode.hpp>

#include <sstream>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CTypeBTNode::CTypeBTNode(
			bool bMutableQualifier, 
			std::string const & sName, 
			std::vector<CTypeBTNode> const & vDomainTypes, 
			std::vector<CTypeBTNode> const & vCodomainTypes) :
			m_sName(sName),
			m_bMutableQualifier(bMutableQualifier),
			m_vDomainTypes(vDomainTypes),
			m_vCodomainTypes(vCodomainTypes)
		{
			// Function types are anonymous
			/*if(m_sName.empty())
			{
				throw std::invalid_argument("Empty type name identifier is invalid!");
			}*/
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CTypeBTNode::GetMutableQualifier() const
		{
			return m_bMutableQualifier;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string const & CTypeBTNode::GetName() const
		{
			return m_sName;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string CTypeBTNode::GetQualifiedName() const
		{
			return /*GetTypeDefinitionBTNode().GetQualifiedName() + "::" + */GetName();	// FIXME: Qualified name for types!
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string CTypeBTNode::GetMangledName() const
		{
			std::stringstream sstr;

			// Print unpure qualifier.
			if(GetMutableQualifier())
			{
				sstr << "~";
			}

			// Print type name.
			sstr << GetQualifiedName();

			// If there are input types
			if(!m_vDomainTypes.empty())
			{
				sstr << "(";

				bool bFirstRet = false;
				for(auto const & type : m_vDomainTypes)
				{
					if(bFirstRet) { bFirstRet = false; }
					else { sstr << ","; }

					sstr << type;
				}

				sstr << ")->";
			}

			// If there are output types
			if(!m_vDomainTypes.empty())
			{
				sstr << "(";

				bool bFirstRet = false;
				for(auto const & type : m_vCodomainTypes)
				{
					if(bFirstRet) { bFirstRet = false; }
					else { sstr << ","; }

					sstr << type;
				}

				sstr << ")";
			}

			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string CTypeBTNode::GetQualifiedMangledName() const
		{
			return /*GetTypeDefinitionBTNode().GetQualifiedMangledName() + "::" + */GetMangledName();	// FIXME: Qualified name for types!
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, CTypeBTNode const & x)
		{
			out << x.GetMangledName();
			return out;
		}
	}
}