#include <unilang/compiler/abt/NamespaceBTNode.hpp>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SNamespaceBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SNamespaceBTNode const & x)
		{
			out << "namespace : "
				<< x.m_sName << std::endl
				<< "{" << std::endl
				<< x.m_vupMetaEntities << std::endl
				<< "}" << std::endl;
			return out;
		}
	}
}