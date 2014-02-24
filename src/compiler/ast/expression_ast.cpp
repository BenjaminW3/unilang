#include <unilang/compiler/ast/expression_ast.hpp>

#include <boost/variant/get.hpp>

#include <sstream>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		primary_expr::primary_expr() : base_type() {}
		primary_expr::primary_expr(std::string val) : base_type(val) {}
		primary_expr::primary_expr(long double val) : base_type(val) {}
		primary_expr::primary_expr(uint64_t val) : base_type(val) {}
		primary_expr::primary_expr(bool val) : base_type(val) {}
		primary_expr::primary_expr(identifier const & val) : base_type(val) {}
		primary_expr::primary_expr(expression const & val) : base_type(val) {}
		primary_expr::primary_expr(primary_expr const & rhs) : base_type(rhs.get()) {}

		std::ostream& operator<<(std::ostream& out, primary_expr const & x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<std::string>(x); break;
				case 1: out << boost::get<long double>(x); break;
				case 2: out << boost::get<uint64_t>(x); break;
				case 3: out << boost::get<bool>(x); break;
				case 4: out << boost::get<identifier>(x); break;
				case 5: out << boost::get<expression>(x); break;
				default: out << "undefined primary_expr"; break;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		operand::operand() : base_type() {}
		operand::operand(primary_expr const & val) : base_type(val) {}
		operand::operand(unary_expr const & val) : base_type(val) {}
		operand::operand(function_call const & val) : base_type(val) {}
		operand::operand(variable_definition const & val) : base_type(val) {}
		//operand::operand(assignment const & val) : base_type(val) {}
		operand::operand(operand const & rhs): base_type(rhs.get()) {}

		std::ostream& operator<<(std::ostream& out, operand const & x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<primary_expr>(x); break;
				case 1: out << boost::get<unary_expr>(x); break;
				case 2: out << boost::get<function_call>(x); break;
				case 3: out << boost::get<variable_definition>(x); break;
				//case 4: out << boost::get<assignment>(x); break;
				default: out << "undefined operand"; break;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, unary_expr const & x)
		{
			out << 
#ifdef TOKEN_ID
				x._uiOperatorID
#else
				static_cast<operators::EOperators>(x._uiOperatorID)
#endif
				<< x._opOperand; return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, operation const & x)
		{
			out << 
#ifdef TOKEN_ID
				x._uiOperatorID
#else
				static_cast<operators::EOperators>(x._uiOperatorID)
#endif
				;
			out << x._opOperand;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, expression const & x)
		{
			out << x._firstOperand; 
			for(operation const & op : x._vRestOperands)
			{
				out << op;
			}
			return out;
		}
		std::ostream& operator<<(std::ostream& out, std::vector<expression> const & x)
		{
			bool bFirst = true;
			for(expression const & ex : x)
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
		std::ostream& operator<<(std::ostream& out, function_call const & x)
		{
			out << x._idfName;
			if(x._bHasUnpureQualifier)
			{
				out << "~";
			}
			out << "(" << x._vArgumentExpressions << ")";
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		variable_type_declaration::variable_type_declaration() :
			_bHasMutableQualifier(false),
			_idfName(/*"unnamed-type"*/)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		variable_type_declaration::variable_type_declaration(identifier const & type_identifier) :
			_bHasMutableQualifier(false),
			_idfName(type_identifier)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		variable_type_declaration::variable_type_declaration(bool bHasMutableQualifier, identifier const & type_identifier) :
			_bHasMutableQualifier(bHasMutableQualifier),
			_idfName(type_identifier)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string variable_type_declaration::build_mangled_name() const
		{
			std::stringstream sstr;
			/*if(_bHasMutableQualifier)
			{
				sstr << "~";
			}*/
			sstr << _idfName;
			return sstr.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, variable_type_declaration const & x)
		{
			if(x._bHasMutableQualifier)
			{
				out << "~";
			}
			out << x._idfName;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, variable_declaration const & x)
		{
			if(x._optionalIdentifier.is_initialized())
			{
				out << x._optionalIdentifier.get().m_sName << ":";
			}
			out << x._type;
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, variable_definition const & x)
		{
			out << x._declaration << "{" << x._vParameterExpressions << "}";
			return out;
		}
	}
}