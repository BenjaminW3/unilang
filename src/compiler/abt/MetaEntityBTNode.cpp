#include <unilang/compiler/abt/MetaEntityBTNode.hpp>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string const & SMetaEntityBTNode::GetName() const
		{
			return m_sName;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<SMetaEntityBTNode> SMetaEntityBTNode::GetParentMetaEntityBTNode() const
		{
			return m_wpParentMetaEntityBTNode.lock();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string SMetaEntityBTNode::GetQualifiedName() const
		{
			auto const * const pVariableDefinitionBTNode(GetParentMetaEntityBTNode().get());
			if(pVariableDefinitionBTNode)
			{
				return pVariableDefinitionBTNode->GetQualifiedName() + "::" + m_sName;
			}
			else
			{
				return m_sName;
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SMetaEntityBTNode const & x)
		{
			CPrinterBTNodeVisitor printVisitor(out);
			x.Accept(printVisitor);
			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, std::vector<std::unique_ptr<SMetaEntityBTNode>> const & x)
		{
			bool bFirstRet = false;
			for(auto const & me : x)
			{
				if(bFirstRet) { bFirstRet = false; }
				else { out << std::endl; }

				auto const * const pMetaEntity(me.get());
				if(pMetaEntity)
				{
					out << *pMetaEntity;
				}
				else
				{
					out << "<Invalid reference to meta entity>" << std::endl;
				}
			}
			return out;
		}
	}
}