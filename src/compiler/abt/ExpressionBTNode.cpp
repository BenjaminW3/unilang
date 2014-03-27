#include <unilang/compiler/abt/ExpressionBTNode.hpp>

#include <unilang/compiler/ast/OperatorsDef.hpp>

#include <unilang/compiler/abt/FunctionBTNode.hpp>
#include <unilang/compiler/abt/VariableBTNode.hpp>

#include <boost/variant/get.hpp>

#include <sstream>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SBooleanTokenExpressionBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SBooleanTokenExpressionBTNode const & x)
		{
			out << x.m_bValue;
			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SUnsignedIntegerExpressionBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SUnsignedIntegerExpressionBTNode const & x)
		{
			out << x.m_uiValue;
			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SFloatingPointExpressionBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SFloatingPointExpressionBTNode const & x)
		{
			out << x.m_fValue;
			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SUnaryExpressionBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		/*bool SUnaryExpressionBTNode::isPure() const
		{
			return m_opOperand.isPure();
		}*/
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SUnaryExpressionBTNode const & x)
		{
			out << x.m_eOperator;

			auto const * const pOperand(x.m_upOperand.get());
			if(pOperand)
			{
				out << *pOperand << std::endl;
			}
			else
			{
				out << "<Invalid unary expression>" << std::endl;
			}
			
			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SBinaryExpressionBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		/*bool SBinaryExpressionBTNode::isPure() const
		{
		return m_opOperand.isPure();
		}*/
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SBinaryExpressionBTNode const & x)
		{

			auto const * const pLeftOperand(x.m_upLeftOperand.get());
			if(pLeftOperand)
			{
				out << *pLeftOperand << std::endl;
			}
			else
			{
				out << "<Invalid left hand side of binary expression>" << std::endl;
			}

			out << x.m_eOperator;

			auto const * const pRightOperand(x.m_upRightOperand.get());
			if(pRightOperand)
			{
				out << *pRightOperand << std::endl;
			}
			else
			{
				out << "<Invalid right hand side of binary expression>" << std::endl;
			}

			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SFunctionCallBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		/*bool SFunctionCallBTNode::isPure() const
		{
			if(m_bHasUnpureQualifier)
			{
				return false;
			}

			// all parameters have to be pure
			for(ast::SExpression const & ex : m_vArgumentExpressions)
			{
				if(!ex.isPure()) {return false;}
			}

			return true;
		}*/
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SFunctionCallBTNode const & x)
		{
			// Print function name.
			auto const * const pCalledFunction(x.m_upCalledFunction.get());
			if(pCalledFunction)
			{
				out << pCalledFunction->GetQualifiedName() << std::endl;
			}
			else
			{
				out << "<Invalid called function>" << std::endl;
			}

			// Print paramters.
			out << "(";

			bool bFirstRet = true;
			for(auto const & upExpression : x.m_vupArgumentExpressions)
			{
				if(bFirstRet) { bFirstRet = false; }
				else { out << ", "; }

				auto const * const pExpression(upExpression.get());
				if(pExpression)
				{
					out << *pExpression;
				}
				else
				{
					out << "<Invalid parameter expression>";
				}
			}

			out  << ")";

			return out;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SVariableDefinitionBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*bool SVariableDefinitionBTNode::isPure() const
		{
			// all parameters have to be pure
			for(ast::SExpression const & ex : m_vParameterExpressions)
			{
				if(!ex.isPure()) {return false;}
			}
			// it has to be non mutable
			return m_declaration.isPure();
		}*/
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SVariableDefinitionBTNode const & x)
		{
			// Print function name.
			auto const * const pVariable(x.m_spVariable.get());
			if(pVariable)
			{
				out << *pVariable;
			}
			else
			{
				out << "<Invalid variable>";
			}

			// Print parameter expressions.
			out << "{";

			bool bFirstRet = true;
			for(auto const & upParameterExpressions : x.m_vupParameterExpressions)
			{
				if(bFirstRet) { bFirstRet = false; }
				else { out << ", "; }

				auto const * const pParameterExpressions(upParameterExpressions.get());
				if(pParameterExpressions)
				{
					out << *pParameterExpressions;
				}
				else
				{
					out << "<Invalid parameter expression>";
				}
			}

			out  << "}";

			return out;
		}
	}
}