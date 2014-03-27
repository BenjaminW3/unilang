#include <unilang/compiler/abt/VariableBTNode.hpp>

#include <unilang/compiler/abt/ExpressionBTNode.hpp>
#include <unilang/compiler/abt/StatementBTNode.hpp>
#include <unilang/compiler/abt/FunctionBTNode.hpp>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CVariableBTNode::CVariableBTNode(std::string const & sName, CTypeBTNode const & type, std::weak_ptr<SVariableDefinitionBTNode> wpVariableDefinitionBTNode) :
			m_sName(sName),
			m_Type(type),
			m_wpVariableDefinitionBTNode(wpVariableDefinitionBTNode)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string const & CVariableBTNode::GetName() const
		{
			return m_sName;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CTypeBTNode const & CVariableBTNode::GetType() const
		{
			return m_Type;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string CVariableBTNode::GetQualifiedName() const
		{
			auto const * const pVariableDefinitionBTNode(GetVariableDefinitionBTNode().get());
			if(pVariableDefinitionBTNode)
			{
				auto const * const pParentMetaEntityBTNode(pVariableDefinitionBTNode->GetParentFunctionDefinitionBTNode().get());
				if(pParentMetaEntityBTNode)
				{
					return pParentMetaEntityBTNode->GetQualifiedName();
				}
				else
				{
					return "<Invalid variable definition syntax node for variable>";
				}
			}
			else
			{
				return "<Invalid variable definition syntax node for variable>";
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<SVariableDefinitionBTNode> CVariableBTNode::GetVariableDefinitionBTNode() const
		{
			return m_wpVariableDefinitionBTNode.lock();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, CVariableBTNode const & x)
		{
			out << x.GetName() << ":" << x.GetType();
			return out;
		}
	}
}