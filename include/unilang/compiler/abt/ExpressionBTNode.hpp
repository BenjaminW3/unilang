#pragma once

#include <unilang/compiler/ast/OperatorsFwd.hpp>
#include <unilang/compiler/abt/StatementBTNode.hpp>

#include <boost/optional.hpp>

#include <ostream>
#include <vector>
#include <memory>

// forward declarations
namespace unilang
{
	namespace abt
	{
		//struct CTypeBTNode;
		struct SFunctionDeclarationBTNode;
		class CVariableBTNode;
	}
}

namespace unilang
{
	namespace abt
	{
		//#########################################################################
		//! An expression.
		//#########################################################################
		struct SExpressionBTNode :
			public SStatementBTNode
		{
			//-------------------------------------------------------------------------
			//! \return The return type of the expression.
			//-------------------------------------------------------------------------
			//virtual std::shared_ptr<CTypeBTNode> GetType() const;
		};

		//#########################################################################
		//! A boolean token.
		//#########################################################################
		struct SBooleanTokenExpressionBTNode :
			public SExpressionBTNode
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

		public:
			bool m_bValue;
		};
		std::ostream& operator<<(std::ostream& out, SBooleanTokenExpressionBTNode const & x);

		//#########################################################################
		//! A integer token.
		//#########################################################################
		struct SUnsignedIntegerExpressionBTNode :
			public SExpressionBTNode
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

		public:
			uint64_t m_uiValue;
		};
		std::ostream& operator<<(std::ostream& out, SUnsignedIntegerExpressionBTNode const & x);

		//#########################################################################
		//! A floating point token.
		//#########################################################################
		struct SFloatingPointExpressionBTNode :
			public SExpressionBTNode
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

		public:
			long double m_fValue;
		};
		std::ostream& operator<<(std::ostream& out, SFloatingPointExpressionBTNode const & x);

		//#########################################################################
		//! A unary expression.
		//#########################################################################
		struct SUnaryExpressionBTNode :
			public SExpressionBTNode
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

		public:
			operators::EOperators m_eOperator;
			std::unique_ptr<SExpressionBTNode> m_upOperand;
		};
		std::ostream& operator<<(std::ostream& out, SUnaryExpressionBTNode const & x);

		//#########################################################################
		//! A binary expression.
		//#########################################################################
		struct SBinaryExpressionBTNode :
			public SExpressionBTNode
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

		public:
			std::unique_ptr<SExpressionBTNode> m_upLeftOperand;
			operators::EOperators m_eOperator;
			std::unique_ptr<SExpressionBTNode> m_upRightOperand;
		};
		std::ostream& operator<<(std::ostream& out, SBinaryExpressionBTNode const & x);

		//#########################################################################
		//! A function call.
		//#########################################################################
		struct SFunctionCallBTNode :
			public SExpressionBTNode
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

		public:
			//std::unique_ptr<SFunctionDeclarationBTNode> m_upCalledFunction;
			std::unique_ptr<SFunctionDefinitionBTNode> m_upCalledFunction;
			std::vector<std::unique_ptr<SExpressionBTNode>> m_vupArgumentExpressions;
		};
		std::ostream& operator<<(std::ostream& out, SFunctionCallBTNode const & x);

		//#########################################################################
		//! A type declaration.
		//#########################################################################
		/*struct SVariableTypeDeclaration :
			public SBindingTreeNode
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SVariableTypeDeclaration();
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SVariableTypeDeclaration(SIdentifier const & type_identifier);
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			SVariableTypeDeclaration(bool bHasMutableQualifier, SIdentifier const & type_identifier);

			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const override final;

			//-------------------------------------------------------------------------
			//! \return If the node is pure.
			//-------------------------------------------------------------------------
			//bool isPure() const override final;
			//-------------------------------------------------------------------------
			//! \return Unique names for different types.
			//-------------------------------------------------------------------------
			std::string build_mangled_name() const;

		public:
			bool m_bHasMutableQualifier;
			SIdentifier m_idfName;
		};
		std::ostream& operator<<(std::ostream& out, SVariableTypeDeclaration const & x);*/

		//#########################################################################
		//! A variable declaration.
		//#########################################################################
		/*struct SVariableDeclaration :
			public SBindingTreeNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) override final;

			//-------------------------------------------------------------------------
			//! \return If the node is pure.
			//-------------------------------------------------------------------------
			//bool isPure() const override final;

		public:
			boost::optional<SIdentifier> m_optionalIdentifier;
			SVariableTypeDeclaration m_type;
		};
		std::ostream& operator<<(std::ostream& out, SVariableDeclaration const & x);*/

		//#########################################################################
		//! A variable definition.
		//#########################################################################
		struct SVariableDefinitionBTNode :
			public SExpressionBTNode
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

		public:
			std::shared_ptr<CVariableBTNode> m_spVariable;
			std::vector<std::unique_ptr<SExpressionBTNode>> m_vupParameterExpressions;
		};
		std::ostream& operator<<(std::ostream& out, SVariableDefinitionBTNode const & x);
	}
}