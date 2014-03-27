#include <unilang/compiler/abt/FunctionBTNode.hpp>

#include <unilang/compiler/abt/StatementBTNode.hpp>
#include <unilang/compiler/abt/ExpressionBTNode.hpp>

#include <unilang/compiler/abt/TypeBTNode.hpp>

#include <sstream>

namespace unilang
{
	namespace abt
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*SFunctionDeclarationBTNode::SFunctionDeclarationBTNode()
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SFunctionDeclarationBTNode::SFunctionDeclarationBTNode(
			std::string const & sName,
			bool bHasUnpureQualifier, 
			std::vector<std::unique_ptr<SVariableTypeDeclarationBTNode>> const & vupParameterTypes,
			std::vector<std::unique_ptr<SVariableTypeDeclarationBTNode>> const & vupReturnTypes) :
			SMetaEntityBTNode	(sName),
			m_bHasUnpureQualifier	(bHasUnpureQualifier),
			m_vupParameterTypes		(vupParameterTypes),
			m_vupReturnTypes		(vupReturnTypes)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SFunctionDeclarationBTNode::SFunctionDeclarationBTNode(SFunctionDefinitionBTNode const & functionDefinitionBTNode) :
			SMetaEntityBTNode(functionDefinitionBTNode.GetName()),
			m_bHasUnpureQualifier	(functionDefinitionBTNode.m_bHasUnpureQualifier)
		{
			for(auto const & decl : functionDefinitionBTNode.m_vupParameterDeclarations)
			{
				m_vupParameterTypes.push_back(decl.m_type);
			}
			for(auto const & def : functionDefinitionBTNode.m_vupReturnValueDefinitions)
			{
				m_vupReturnTypes.push_back(def.m_declaration.m_type);
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SFunctionDeclarationBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		//bool SFunctionDeclarationBTNode::isPure() const
		//{
		//	return false;
		//}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string SFunctionDeclarationBTNode::build_mangled_name() const
		{
			std::stringstream sstr;

			sstr << GetName();

			sstr << ":";
			if(m_bHasUnpureQualifier)
			{
				sstr << "~";
			}
			sstr << "(";
			bool bFirstArg = true;
			for(auto const & upParamType : m_vupParameterTypes)
			{
				if(bFirstArg){bFirstArg = false;}
				else{sstr << ",";}

				sstr << upParamType->build_mangled_name();
			}
			sstr << ")";
			// The return value is not part of the mangled name because on the call site of a function the return type is irrelevant and is determined by overload resolution of the argument types.
			// So overloads on the return values only are not allowed.
			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SFunctionDeclarationBTNode const & x)
		{
			out << "?";

			out << x.GetName();

			out << ":";
			if(x.m_bHasUnpureQualifier)
			{
				out << " ~";
			}

			out << "(";
			bool bFirstArg = true;
			for(auto const & upParamType : x.m_vupParameterTypes)
			{
				if(bFirstArg){bFirstArg = false;}
				else { out << ", "; }

				auto const * const pParameterType(upParamType.get());
				if(pParameterType)
				{
					out << *pParameterType;
				}
				else
				{
					out << "<Invalid function declaration parameter type>";
				}
			}
			out << ") -> ";

			out << '(';
			bool bFirstRet = true;
			for(auto const & upReturnType : x.m_vupReturnTypes)
			{
				if(bFirstRet){bFirstRet = false;}
				else { out << ", "; }

				auto const * const pReturnType(upReturnType.get());
				if(pReturnType)
				{
					out << *pReturnType;
				}
				else
				{
					out << "<Invalid function declaration return type>";
				}
			}
			out << ")";

			return out;
		}*/

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void SFunctionDefinitionBTNode::Accept(CBTNodeVisitor const & visitor) const
		{
			visitor.Visit(*this);
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*bool SFunctionDefinitionBTNode::isPure() const
		{
			if(m_bHasUnpureQualifier){return false;}
			for(abt::SVariableDefinition const & ret : m_vReturnExpressions)
			{
				if(!ret.isPure()) {return false;}
			}
			for(abt::SVariableDeclaration const & arg : m_vParameterDeclarations)
			{
				if(!arg.isPure()) {return false;}
			}
			return m_body.isPure();
		}*/
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string SFunctionDefinitionBTNode::GetMangledName() const
		{
			std::stringstream sstr;

			// Print function name.
			sstr << GetName();

			sstr << ":";

			// Print parameter types.
			auto const * const pTypeNode(m_upTypeNode.get());
			if(pTypeNode)
			{
				if(pTypeNode->GetMutableQualifier())
				{
					sstr << "~";
				}

				sstr << '(';

				bool bFirstRet = true;
				for(auto const & DomainType : pTypeNode->m_vDomainTypes)
				{
					if(bFirstRet) { bFirstRet = false; }
					else { sstr << ", "; }

					sstr << DomainType;
				}

				sstr << ')';
			}
			else
			{
				throw std::runtime_error("Invalid function type! GetMangledName can not be called on a function with invalid type.");
			}

			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string SFunctionDefinitionBTNode::GetQualifiedMangledName() const
		{
			std::string sQualifiedMangledName("::" + GetMangledName());
			auto const * const pVariableDefinitionBTNode(GetParentMetaEntityBTNode().get());
			if(pVariableDefinitionBTNode)
			{
				return pVariableDefinitionBTNode->GetQualifiedName() + sQualifiedMangledName;
			}
			else
			{
				return sQualifiedMangledName;
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SFunctionDefinitionBTNode const & x)
		{
			// Print function name.
			out << x.GetName();

			out << ":";

			// Print parameter types.
			auto const * const pTypeNode(x.m_upTypeNode.get());
			if(pTypeNode)
			{
				if(pTypeNode->GetMutableQualifier())
				{
					out << "~";
				}

				out << '(';

				bool bFirstRet = true;
				for(auto const & DomainType : pTypeNode->m_vDomainTypes)
				{
					if(bFirstRet) { bFirstRet = false; }
					else { out << ", "; }

					out << DomainType;
				}

				out << ')';
			}
			else
			{
				out << "<Invalid function type>" << std::endl;
			}

			/*auto const * const pTypeNode(x.m_upTypeNode.get());
			if(pTypeNode)
			{
				out << *pTypeNode << std::endl;
			}
			else
			{
				out << "<Invalid function type>" << std::endl;
			}*/

			// Print return value definition list.
			out << '(';
			bool bFirstRet = true;
			for(auto const & upReturnDef : x.m_vupReturnValueDefinitions)
			{
				if(bFirstRet){bFirstRet = false;}
				else { out << ", "; }

				auto const * const pReturnValueDefinition(upReturnDef.get());
				if(pReturnValueDefinition)
				{
					out << *pReturnValueDefinition;
				}
				else
				{
					out << "<Invalid function definition return value definition>";
				}
			}
			out << ")";

			// Print body statement block.
			out << '{' << std::endl;
			auto const * const pBodyStatementBlock(x.m_upBodyStatementBlock.get());
			if(pBodyStatementBlock)
			{
				out << *pBodyStatementBlock << std::endl;
			}
			else
			{
				out << "<Invalid function definition body statement block>" << std::endl;
			}
			out << '}' << std::endl;

			return out;
		}
	}
}