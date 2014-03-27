#pragma once

//#include <string>
#include <ostream>

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace unilang
{
	namespace abt
	{
		struct SRootBTNode;
		struct SNamespaceBTNode;
		struct SFunctionDefinitionBTNode;
		struct SStatementBlockBTNode;
		struct SIfStatementBTNode;
		struct SAssignmentStatementBTNode;
		struct SBooleanTokenExpressionBTNode;
		struct SUnsignedIntegerExpressionBTNode;
		struct SFloatingPointExpressionBTNode;
		struct SUnaryExpressionBTNode;
		struct SBinaryExpressionBTNode;
		struct SFunctionCallBTNode;
		struct SVariableDefinitionBTNode;
	}
}

namespace unilang
{
	namespace abt
	{
		//#########################################################################
		//! The visitor class.
		//#########################################################################
		class CBTNodeVisitor
		{
		public:
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SRootBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SNamespaceBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SFunctionDefinitionBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SStatementBlockBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SIfStatementBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SAssignmentStatementBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SBooleanTokenExpressionBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SUnsignedIntegerExpressionBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SFloatingPointExpressionBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SUnaryExpressionBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SBinaryExpressionBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SFunctionCallBTNode const & node) const = 0;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SVariableDefinitionBTNode const & node) const = 0;
		};

		//#########################################################################
		//! The visitor class.
		//#########################################################################
		class CPrinterBTNodeVisitor :
			public CBTNodeVisitor,
			boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CPrinterBTNodeVisitor(std::ostream & os);

			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SRootBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SNamespaceBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation streaming out the node.
			//-------------------------------------------------------------------------
			virtual void Visit(SFunctionDefinitionBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SStatementBlockBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SIfStatementBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SAssignmentStatementBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SBooleanTokenExpressionBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SUnsignedIntegerExpressionBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SFloatingPointExpressionBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SUnaryExpressionBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SBinaryExpressionBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SFunctionCallBTNode const & node) const override final;
			//-------------------------------------------------------------------------
			//! Visit implementation.
			//-------------------------------------------------------------------------
			virtual void Visit(SVariableDefinitionBTNode const & node) const override final;

		private:
			std::ostream & m_os;
		};

		//#########################################################################
		//! The base class for all abt objects.
		//! It holds the id for annotations.
		//#########################################################################
		struct SBindingTreeNode
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			SBindingTreeNode();
			
			//-------------------------------------------------------------------------
			//! \return If the node is pure.
			//-------------------------------------------------------------------------
			//virtual bool isPure() const = 0;
			
			//-------------------------------------------------------------------------
			//! \return If the node is valid.
			//-------------------------------------------------------------------------
			//virtual bool IsCompleteAndValid() const = 0;

			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const = 0;

			//-------------------------------------------------------------------------
			//! The method to visit the nodes children.
			//-------------------------------------------------------------------------
			//virtual void VisitChildren(CBTNodeVisitor const & visitor) const = 0;

			size_t m_uiID;	//!< The annotaion ID.
		};
	}
}