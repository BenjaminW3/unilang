#pragma once

#include <unilang/compiler/abt/MetaEntityBTNode.hpp>

#include <memory>
#include <ostream>
#include <vector>

// forward declarations
namespace unilang
{
	namespace abt
	{
		//struct SVariableTypeDeclarationBTNode;
		//struct SVariableDeclarationBTNode;
		struct SVariableDefinitionBTNode;
		struct SStatementBlockBTNode;
		class CTypeBTNode;
	}
}

namespace unilang
{
	namespace abt
	{
		//#########################################################################
		//! A function consisting of its declaration and its body.
		//#########################################################################
		struct SFunctionDefinitionBTNode :
			public SMetaEntityBTNode
		{
		public:
			//-------------------------------------------------------------------------
			//! The method to visit the nodes.
			//-------------------------------------------------------------------------
			virtual void Accept(CBTNodeVisitor const & visitor) const override final;

			//-------------------------------------------------------------------------
			//! \return The mangled name of the function.
			//! Throws an exception if it is not valid.
			//-------------------------------------------------------------------------
			std::string GetMangledName() const;
			//-------------------------------------------------------------------------
			//! \return The qualified mangled name of the function.
			//! Throws an exception if it is not valid.
			//-------------------------------------------------------------------------
			std::string GetQualifiedMangledName() const;

		public:
			std::vector<std::unique_ptr<SVariableDefinitionBTNode>> m_vupReturnValueDefinitions;
			std::unique_ptr<SStatementBlockBTNode> m_upBodyStatementBlock;

			// Secondary member build from primary members.
			std::unique_ptr<CTypeBTNode> m_upTypeNode;
		};
		std::ostream& operator<<(std::ostream& out, SFunctionDefinitionBTNode const & x);
	}
}