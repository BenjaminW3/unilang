#include <unilang/compiler/abt/BindingTreeNode.hpp>

#include <unilang/compiler/abt/RootBTNode.hpp>
#include <unilang/compiler/abt/NamespaceBTNode.hpp>
#include <unilang/compiler/abt/FunctionBTNode.hpp>
#include <unilang/compiler/abt/StatementBTNode.hpp>
#include <unilang/compiler/abt/ExpressionBTNode.hpp>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CPrinterBTNodeVisitor::CPrinterBTNodeVisitor(std::ostream & os) :
			m_os(os)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SRootBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SNamespaceBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SFunctionDefinitionBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SStatementBlockBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SIfStatementBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SAssignmentStatementBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SBooleanTokenExpressionBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SUnsignedIntegerExpressionBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SFloatingPointExpressionBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SUnaryExpressionBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SBinaryExpressionBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SFunctionCallBTNode const & node) const
		{
			m_os << node;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void CPrinterBTNodeVisitor::Visit(SVariableDefinitionBTNode const & node) const
		{
			m_os << node;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SBindingTreeNode::SBindingTreeNode() :
			m_uiID(static_cast<size_t>(-1))
		{
		}
	}
}