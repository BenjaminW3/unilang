#include <unilang/compiler/abt/RootBTNode.hpp>

#include <unilang/compiler/abt/MetaEntityBTNode.hpp>

#include <unilang/compiler/ast/AST.hpp>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SRootBTNode::SRootBTNode(ast::module const & module)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SRootBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SRootBTNode const & x)
		{
			out << x.m_vupMetaEntities;
			return out;
		}
	}
}