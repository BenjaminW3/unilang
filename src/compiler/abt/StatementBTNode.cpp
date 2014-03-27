#include <unilang/compiler/abt/StatementBTNode.hpp>

#include <unilang/compiler/abt/ExpressionBTNode.hpp>
#include <unilang/compiler/abt/VariableBTNode.hpp>

#include <boost/variant/get.hpp>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<SFunctionDefinitionBTNode> SStatementBTNode::GetParentFunctionDefinitionBTNode() const
		{
			return m_wpParentFunctionDefinitionBTNode.lock();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SStatementBTNode const & x)
		{
			CPrinterBTNodeVisitor printVisitor(out);
			x.Accept(printVisitor);
			return out;
		}
		
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SStatementBlockBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*bool SStatementBlockBTNode::isPure() const
		{
			for(ast::SStatement const & st : m_vupStatements)
			{
				if(!st.isPure()) {return false;}
			}
			return true;
		}*/
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SStatementBlockBTNode const & x)
		{
			for(auto const & upStatement : x.m_vupStatements)
			{
				auto const * const pStatement(upStatement.get());
				if(pStatement)
				{
					out << *pStatement;
				}
				else
				{
					out << "<Invalid statement in statement block>";
				}
				out << std::endl;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SIfStatementBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*bool SIfStatementBTNode::isPure() const
		{
			return m_upConditionExpression.isPure() && m_upThenStatementBlock.isPure() && (!m_upElseStatementBlock.get() || m_upElseStatementBlock->isPure());
		}*/
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SIfStatementBTNode const & x)
		{
			// Print Condition.
			out << "if(";
			
			auto const * const pConditionExpression(x.m_upConditionExpression.get());
			if(pConditionExpression)
			{
				out << *pConditionExpression;
			}
			else
			{
				out << "<Invalid if condition expression>";
			}

			out << ")" << std::endl;

			// Print then block.
			out << "{" << std::endl;

			auto const * const pThenStatementBlock(x.m_upThenStatementBlock.get());
			if(pThenStatementBlock)
			{
				out << *pThenStatementBlock;
			}
			else
			{
				out << "<Invalid then statement block>";
			}

			out << "}" << std::endl;

			// Print else block.
			auto const * const pElseStatementBlock(x.m_upElseStatementBlock.get());
			if(pElseStatementBlock)
			{
				out << "else" << std::endl
					<< "{" << std::endl
					<< *pElseStatementBlock
					<< "}" << std::endl;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<CVariableBTNode> SAssignmentStatementBTNode::GetLHSVariable() const
		{
			return m_wpLHSVariable.lock();
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		/*bool SAssignmentStatementBTNode::isPure() const
		{
			return false;
		}*/
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SAssignmentStatementBTNode const & x)
		{
			// Print LHS variable.
			auto const * const pLHSVariable(x.GetLHSVariable().get());
			if(pLHSVariable)
			{
				out << *pLHSVariable;
			}
			else
			{
				out << "<Invalid left hand side assignement variable>";
			}

			// Print operator.
			out << 
#ifdef TOKEN_ID
				x.m_uiOperatorID;
#else
				static_cast<operators::EOperators>(x.m_uiOperatorID);
#endif

			// Print RHS expression.
			auto const * const pRHSExpression(x.m_upRHSExpression.get());
			if(pRHSExpression)
			{
				out << *pRHSExpression;
			}
			else
			{
				out << "<Invalid right hand side assignment expression>";
			}
			
			return out;
		}
	}
}