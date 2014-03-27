#pragma once

#include <unilang/compiler/abt/BindingTreeNode.hpp>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace unilang
{
	namespace abt
	{
		//#########################################################################
		//! A SMetaEntityBTNode is a member of a namespace or a namespace itself.
		//#########################################################################
		struct SMetaEntityBTNode :
			public SBindingTreeNode
		{
		public:
			//-------------------------------------------------------------------------
			//! \return The name.
			//-------------------------------------------------------------------------
			std::string const & GetName() const;
			//-------------------------------------------------------------------------
			//! \return The syntax node defining the variable.
			//-------------------------------------------------------------------------
			std::shared_ptr<SMetaEntityBTNode> GetParentMetaEntityBTNode() const;	// FIXME: Functions in functions are currently not allowed!
			//-------------------------------------------------------------------------
			//! \return The qualified name of the meta entity syntax node.
			//-------------------------------------------------------------------------
			std::string GetQualifiedName() const;

		public:
			std::string m_sName;
			std::weak_ptr<SMetaEntityBTNode> m_wpParentMetaEntityBTNode;
		};
		std::ostream& operator<<(std::ostream& out, SMetaEntityBTNode const & x);
		std::ostream& operator<<(std::ostream& out, std::vector<std::unique_ptr<SMetaEntityBTNode>> const & x);
	}
}