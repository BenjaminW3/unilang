#pragma once

#include <unilang/compiler/abt/MetaEntityBTNode.hpp>

#include <memory>
#include <ostream>
#include <vector>

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining all the decorated abstract syntax tree objects.
	//-------------------------------------------------------------------------
	namespace abt
	{
		//#########################################################################
		//! A namespace declaration.
		//#########################################################################
		struct SNamespaceBTNode :
			public SMetaEntityBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const override final;

		public:
			std::string m_sName;

			std::vector<std::unique_ptr<SMetaEntityBTNode>> m_vupMetaEntities;
		};
		std::ostream& operator<<(std::ostream& out, SNamespaceBTNode const & x);
	}
}