#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace unilang
{
	namespace abt
	{
		//#########################################################################
		//! A type.
		//#########################################################################
		class CTypeBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CTypeBTNode(
				bool bMutableQualifier, 
				std::string const & sName, 
				std::vector<CTypeBTNode> const & vDomainTypes, 
				std::vector<CTypeBTNode> const & vCodomainTypes);

			//-------------------------------------------------------------------------
			//! \return If the type has a mutable qualifier.
			//-------------------------------------------------------------------------
			bool GetMutableQualifier() const;

			//-------------------------------------------------------------------------
			//! \return The syntax node defining the type.
			//-------------------------------------------------------------------------
			//STypeDefinitionBTNode const & GetTypeDefinitionBTNode() const;

			//-------------------------------------------------------------------------
			//! \return The name of the type.
			//-------------------------------------------------------------------------
			std::string const & GetName() const;
			//-------------------------------------------------------------------------
			//! \return The qualified name of the type.
			//-------------------------------------------------------------------------
			std::string GetQualifiedName() const;

			//-------------------------------------------------------------------------
			//! \return The mangled name of the type.
			//-------------------------------------------------------------------------
			std::string GetMangledName() const;
			//-------------------------------------------------------------------------
			//! \return The qualified mangled name of the type.
			//-------------------------------------------------------------------------
			std::string GetQualifiedMangledName() const;

		public:
			bool m_bMutableQualifier;
			std::string m_sName;
			std::vector<CTypeBTNode> m_vDomainTypes;	// Input
			std::vector<CTypeBTNode> m_vCodomainTypes;	// Output

			//std::weak_ptr<SVariableDefinitionBTNode> m_wpTypeDefinitionBTNode;

			//std::vector<std::weak_ptr<SVariableDefinitionBTNode>> m_vwpTypeUsageBTNodes;
		};
		std::ostream& operator<<(std::ostream& out, CTypeBTNode const & x);
	}
}