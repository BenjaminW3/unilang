#pragma once

#include <unilang/compiler/abt/TypeBTNode.hpp>

#include <memory>
#include <ostream>
#include <string>

// forward declarations
namespace unilang
{
	namespace abt
	{
		struct SVariableDefinitionBTNode;
	}
}

namespace unilang
{
	namespace abt
	{
		//#########################################################################
		//! A variable.
		//#########################################################################
		class CVariableBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CVariableBTNode(std::string const & sName, CTypeBTNode const & type, std::weak_ptr<SVariableDefinitionBTNode> wpVariableDefinitionBTNode);

			//-------------------------------------------------------------------------
			//! \return The type of the variable.
			//-------------------------------------------------------------------------
			CTypeBTNode const & GetType() const;

			//-------------------------------------------------------------------------
			//! \return The name of the variable.
			//-------------------------------------------------------------------------
			std::string const & GetName() const;
			//-------------------------------------------------------------------------
			//! \return The qualified name of the variable.
			//-------------------------------------------------------------------------
			std::string GetQualifiedName() const;

			//-------------------------------------------------------------------------
			//! \return The mangled name of the variable.
			//-------------------------------------------------------------------------
			/*std::string const & GetMangledName() const;
			//-------------------------------------------------------------------------
			//! \return The qualified mangled name of the variable.
			//-------------------------------------------------------------------------
			std::string const & GetQualifiedMangledName() const;*/

			//-------------------------------------------------------------------------
			//! \return The syntax node defining the variable.
			//-------------------------------------------------------------------------
			std::shared_ptr<SVariableDefinitionBTNode> GetVariableDefinitionBTNode() const;

		public:
			std::string m_sName;
			CTypeBTNode m_Type;
			//llvm::AllocaInst * m_pMem;

			std::weak_ptr<SVariableDefinitionBTNode> m_wpVariableDefinitionBTNode;
		};
		std::ostream& operator<<(std::ostream& out, CVariableBTNode const & x);
	}
}