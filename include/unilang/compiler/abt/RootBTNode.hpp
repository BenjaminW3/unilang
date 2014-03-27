#pragma once

#include <unilang/compiler/abt/BindingTreeNode.hpp>

#include <memory>
#include <ostream>
#include <vector>

// forward declarations
namespace unilang
{
	namespace ast
	{
		struct module;
	}
	namespace abt
	{
		struct SMetaEntityBTNode;
	}
}

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining all the decorated abstract syntax tree objects.
	//-------------------------------------------------------------------------
	namespace abt
	{
		//#########################################################################
		//! The root of the decorated abstract syntax tree.
		//#########################################################################
		struct SRootBTNode :
			public SBindingTreeNode
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SRootBTNode(ast::module const & module);

			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const override final;

		public:
			std::vector<std::unique_ptr<SMetaEntityBTNode>> m_vupMetaEntities;
		};
		std::ostream& operator<<(std::ostream& out, SRootBTNode const & x);
	}
}