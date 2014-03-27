#pragma once

#include <unilang/compiler/ast/OperatorsFwd.hpp>
#include <unilang/compiler/abt/BindingTreeNode.hpp>

#include <boost/optional.hpp>

#include <memory>
#include <vector>
#include <ostream>

// forward declarations
namespace unilang
{
	namespace abt
	{
		struct SFunctionDefinitionBTNode;
		struct SExpressionBTNode;
		class CVariableBTNode;
	}
}

namespace unilang
{
	namespace abt
	{
		//#########################################################################
		//! A statement.
		//#########################################################################
		struct SStatementBTNode :
			public SBindingTreeNode
		{
			//-------------------------------------------------------------------------
			//! \return The syntax node defining the variable.
			//-------------------------------------------------------------------------
			std::shared_ptr<SFunctionDefinitionBTNode> GetParentFunctionDefinitionBTNode() const;

		public:
			std::weak_ptr<SFunctionDefinitionBTNode> m_wpParentFunctionDefinitionBTNode;
		};
		std::ostream& operator<<(std::ostream& out, SStatementBTNode const & x);

		//#########################################################################
		//! A statement list of multiple statements.
		//#########################################################################
		struct SStatementBlockBTNode :
			public SStatementBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const override final;

		public:
			std::vector<std::unique_ptr<SStatementBTNode>> m_vupStatements;
		};
		std::ostream& operator<<(std::ostream& out, SStatementBlockBTNode const & x);

		//#########################################################################
		//! If-statement.
		//#########################################################################
		struct SIfStatementBTNode :
			public SStatementBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const override final;

		public:
			std::unique_ptr<SExpressionBTNode> m_upConditionExpression;
			std::unique_ptr<SStatementBlockBTNode> m_upThenStatementBlock;
			std::unique_ptr<SStatementBlockBTNode> m_upElseStatementBlock;
		};
		std::ostream& operator<<(std::ostream& out, SIfStatementBTNode const & x);

		//#########################################################################
		//! While statement.
		//#########################################################################
		/*struct SWhileStatementBTNode :
			public SStatementBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) override final;

		public:
			SExpressionBTNode m_expCondition;
			SStatementBlockBTNode m_body;
		};*/

		//#########################################################################
		//! Return statement.
		//#########################################################################
		/*struct SReturnStatementBTNode :
			public SStatementBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) override final;
		};*/

		//#########################################################################
		//! Assignment consists of a identifier, the operator and an expression.
		//#########################################################################
		struct SAssignmentStatementBTNode :
			public SStatementBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const override final;

			//-------------------------------------------------------------------------
			//! \return If the node is pure.
			//-------------------------------------------------------------------------
			//bool isPure() const override final;

			//-------------------------------------------------------------------------
			//! \return The variable the vaue is assigned to.
			//-------------------------------------------------------------------------
			std::shared_ptr<CVariableBTNode> GetLHSVariable() const;

		public:
			std::weak_ptr<CVariableBTNode> m_wpLHSVariable;
			operators::EOperators m_uiOperatorID;
			std::unique_ptr<SExpressionBTNode> m_upRHSExpression;
		};
		std::ostream& operator<<(std::ostream& out, SAssignmentStatementBTNode const & x);
	}
}