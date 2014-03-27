#include <unilang/compiler/ast/Expression.hpp>

#include <unilang/compiler/ast/OperatorsDef.hpp>		// assignment default constructor

#include <boost/variant/get.hpp>

#include <sstream>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		SPrimaryExpr::SPrimaryExpr() : base_type() {}
		SPrimaryExpr::SPrimaryExpr(std::string const & val) : base_type(val) {}
		SPrimaryExpr::SPrimaryExpr(lexer::SFloatLiteral const & val) : base_type(val) {}
		SPrimaryExpr::SPrimaryExpr(lexer::SIntLiteral const & val) : base_type(val) {}
		SPrimaryExpr::SPrimaryExpr(bool const & val) : base_type(val) {}
		SPrimaryExpr::SPrimaryExpr(SIdentifier const & val) : base_type(val) {}
		SPrimaryExpr::SPrimaryExpr(SExpression const & val) : base_type(val) {}
		SPrimaryExpr::SPrimaryExpr(SPrimaryExpr const & rhs) : base_type(rhs.get()) {}

		std::ostream& operator<<(std::ostream& out, SPrimaryExpr const & x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<std::string>(x); break;
				case 1: out << boost::get<lexer::SFloatLiteral>(x); break;
				case 2: out << boost::get<lexer::SIntLiteral>(x); break;
				case 3: out << boost::get<bool>(x); break;
				case 4: out << boost::get<SIdentifier>(x); break;
				case 5: out << boost::get<SExpression>(x); break;
				default: out << "undefined SPrimaryExpr"; break;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		SOperand::SOperand() : base_type() {}
		SOperand::SOperand(SPrimaryExpr const & val) : base_type(val) {}
		SOperand::SOperand(SUnaryExpr const & val) : base_type(val) {}
		SOperand::SOperand(SFunctionCall const & val) : base_type(val) {}
		SOperand::SOperand(SVariableDefinition const & val) : base_type(val) {}
		//SOperand::SOperand(SAssignment const & val) : base_type(val) {}
		SOperand::SOperand(SOperand const & rhs) : base_type(rhs.get()) {}

		std::ostream& operator<<(std::ostream& out, SOperand const & x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<SPrimaryExpr>(x); break;
				case 1: out << boost::get<SUnaryExpr>(x); break;
				case 2: out << boost::get<SFunctionCall>(x); break;
				case 3: out << boost::get<SVariableDefinition>(x); break;
				//case 4: out << boost::get<SAssignment>(x); break;
				default: out << "undefined operand"; break;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SUnaryExpr const & x)
		{
			out << 
#ifdef TOKEN_ID
				x.m_uiOperatorID
#else
				static_cast<operators::EOperators>(x.m_uiOperatorID)
#endif
				<< x.m_opOperand; return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SOperation const & x)
		{
			out << 
#ifdef TOKEN_ID
				x.m_uiOperatorID
#else
				static_cast<operators::EOperators>(x.m_uiOperatorID)
#endif
				;
			out << x.m_opOperand;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SExpression const & x)
		{
			out << x.m_firstOperand; 
			for(SOperation const & op : x.m_vRestOperands)
			{
				out << op;
			}
			return out;
		}
		std::ostream& operator<<(std::ostream& out, std::vector<SExpression> const & x)
		{
			bool bFirst = true;
			for(SExpression const & ex : x)
			{
				if(bFirst){bFirst = false;}
				else{out << ", ";}

				out << ex;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SFunctionCall const & x)
		{
			out << x.m_idfName;
			if(x.m_bHasUnpureQualifier)
			{
				out << "~";
			}
			out << "(" << x.m_vArgumentExpressions << ")";
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		SVariableTypeDeclaration::SVariableTypeDeclaration() :
			m_bHasMutableQualifier(false),
			m_idfName(/*"unnamed-type"*/)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SVariableTypeDeclaration::SVariableTypeDeclaration(SIdentifier const & type_identifier) :
			m_bHasMutableQualifier(false),
			m_idfName(type_identifier)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		SVariableTypeDeclaration::SVariableTypeDeclaration(bool bHasMutableQualifier, SIdentifier const & type_identifier) :
			m_bHasMutableQualifier(bHasMutableQualifier),
			m_idfName(type_identifier)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string SVariableTypeDeclaration::build_mangled_name() const
		{
			std::stringstream sstr;
			/*if(m_bHasMutableQualifier)
			{
				sstr << "~";
			}*/
			sstr << m_idfName;
			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SVariableTypeDeclaration const & x)
		{
			if(x.m_bHasMutableQualifier)
			{
				out << "~";
			}
			out << x.m_idfName;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SVariableDeclaration const & x)
		{
			if(x.m_optionalIdentifier.is_initialized())
			{
				out << x.m_optionalIdentifier.get().m_sName << ":";
			}
			out << x.m_type;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SVariableDefinition const & x)
		{
			out << x.m_declaration << "{" << x.m_vParameterExpressions << "}";
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		SAssignment::SAssignment() :
			m_lhs(/*"unnamed-identifier"*/),
			m_uiOperatorID(static_cast<OPERATOR_TYPE>(operators::EOperators::assign)),
			m_rhs()
		{
		}
		SAssignment::SAssignment(SIdentifier lhs, OPERATOR_TYPE op, SExpression rhs) :
			m_lhs(lhs),
			m_uiOperatorID(op),
			m_rhs(rhs)
		{
		}
		std::ostream& operator<<(std::ostream& out, SAssignment const & x)
		{
			out << x.m_lhs <<
#ifdef TOKEN_ID
				x.m_uiOperatorID
#else
				static_cast<operators::EOperators>(x.m_uiOperatorID)
#endif
				<< x.m_rhs; return out;
		}
	}
}