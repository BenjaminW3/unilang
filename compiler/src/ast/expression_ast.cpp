#include "expression_ast.hpp"

#include "operators_def.hpp"		// assignment default constructor

#include <boost/variant/get.hpp>

namespace unilang 
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		primary_expr::primary_expr() : base_type() {}
		primary_expr::primary_expr(long double val) : base_type(val) {}
		primary_expr::primary_expr(uint64_t val) : base_type(val) {}
		primary_expr::primary_expr(bool val) : base_type(val) {}
		primary_expr::primary_expr(identifier const& val) : base_type(val) {}
		primary_expr::primary_expr(expression const& val) : base_type(val) {}
		primary_expr::primary_expr(primary_expr const& rhs) : base_type(rhs.get()) {}

		bool primary_expr::isPure() const
		{
			switch(get().which())
			{
				case 0: return true; break;
				case 1: return true; break;
				case 2: return true; break;
				case 3: return boost::get<identifier>(*this).isPure(); break;
				case 4: return boost::get<expression>(*this).isPure(); break;
				default: throw std::runtime_error("undefined-primary_expr"); break;
			}
		}
		std::ostream& operator<<(std::ostream& out, primary_expr const& x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<long double>(x); break;
				case 1: out << boost::get<uint64_t>(x); break;
				case 2: out << boost::get<bool>(x); break;
				case 3: out << boost::get<identifier>(x); break;
				case 4: out << boost::get<expression>(x); break;
				default: out << "undefined-primary_expr"; break;
			}
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		operand::operand() : base_type() {}
		operand::operand(primary_expr const& val) : base_type(val) {}
		operand::operand(unary_expr const& val) : base_type(val) {}
		operand::operand(function_call const& val) : base_type(val) {}
		operand::operand(variable_definition const& val) : base_type(val) {}
		operand::operand(assignment const& val) : base_type(val) {}
		operand::operand(operand const& rhs): base_type(rhs.get()) {}

		bool operand::isPure() const
		{
			switch(get().which())
			{
				case 0: return boost::get<primary_expr>(*this).isPure(); break;
				case 1: return boost::get<unary_expr>(*this).isPure(); break;
				case 2: return boost::get<function_call>(*this).isPure(); break;
				case 3: return boost::get<variable_definition>(*this).isPure(); break;
				case 4: return boost::get<assignment>(*this).isPure(); break;
				default: throw std::runtime_error("undefined-operand"); break;
			}
		}
		std::ostream& operator<<(std::ostream& out, operand const& x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<primary_expr>(x); break;
				case 1: out << boost::get<unary_expr>(x); break;
				case 2: out << boost::get<function_call>(x); break;
				case 3: out << boost::get<variable_definition>(x); break;
				case 4: out << boost::get<assignment>(x); break;
				default: out << "undefined-operand"; break;
			}
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool unary_expr::isPure() const
		{
			return _operand.isPure();
		}
		std::ostream& operator<<(std::ostream& out, unary_expr const& x)
		{
			out << 
#ifdef TOKEN_ID
				x._operator
#else
				static_cast<operators::EOperators>(x._operator)
#endif
				<< x._operand; return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool operation::isPure() const
		{
			return _operand.isPure();
		}
		std::ostream& operator<<(std::ostream& out, operation const& x)
		{
			out << 
#ifdef TOKEN_ID
				x._operator
#else
				static_cast<operators::EOperators>(x._operator)
#endif
				;
			out << x._operand;
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool expression::isPure() const
		{
			if(!_first.isPure()){return false;}
			// all rest have to be pure
			for(ast::operation const & op : _rest)
			{
				if(!op.isPure()) {return false;}
			}
			return true;
		}
		std::ostream& operator<<(std::ostream& out, expression const& x)
		{
			out << x._first; 
			for(operation const & op : x._rest)
			{
				out << op;
			}
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool function_call::isPure() const
		{
			// FIXME: pure test for function itself

			// all parameters have to be pure
			for(ast::expression const & ex : _lArgumentExpressions)
			{
				if(!ex.isPure()) {return false;}
			}

			return true;
		}
		std::ostream& operator<<(std::ostream& out, function_call const& x)
		{
			out << x._identifier << "(";
			bool bFirst = true;
			for(expression const & ex : x._lArgumentExpressions)
			{
				if(bFirst){bFirst = false;}
				else{out << ", ";}

				out << ex;
			}
			out << ")";
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		type_declaration::type_declaration()
			:_bHasMutableQualifier(false),
			_identifier(/*"unnamed-type"*/)
		{
		}
		type_declaration::type_declaration(identifier const & type_identifier)
			:_bHasMutableQualifier(false),
			_identifier(type_identifier)
		{
		}
		type_declaration::type_declaration(bool bHasMutableQualifier, identifier const & type_identifier)
			:_bHasMutableQualifier(bHasMutableQualifier),
			_identifier(type_identifier)
		{
		}
		bool type_declaration::isPure() const
		{
			// it has to be non mutable
			return (!_bHasMutableQualifier);
		}
		std::ostream& operator<<(std::ostream& out, type_declaration const& x)
		{
			if(x._bHasMutableQualifier)
			{
				out << "~";
			}
			out << x._identifier;
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool variable_declaration::isPure() const
		{
			// it has to be non mutable
			return _type.isPure();
		}
		std::ostream& operator<<(std::ostream& out, variable_declaration const& x)
		{
			if(x._optionalIdentifier.is_initialized())
			{
				out << x._optionalIdentifier.get()._name << ":";
			}
			out << x._type;
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool variable_definition::isPure() const
		{
			// FIXME: constructor has to be pure!

			// all parameters have to be pure
			for(ast::expression const & ex : _lParameterExpressions)
			{
				if(!ex.isPure()) {return false;}
			}
			// it has to be non mutable
			return _declaration.isPure();
		}
		std::ostream& operator<<(std::ostream& out, variable_definition const& x)
		{
			out << x._declaration << "{";
			for(expression const & ex : x._lParameterExpressions)
			{
				out << ex;
			}
			out << "}";
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		assignment::assignment()
			:_lhs(/*"unnamed-identifier"*/),
			_operator(static_cast<OPERATOR_TYPE>(operators::EOperators::assign)),
			_rhs()
		{
		}
		assignment::assignment(identifier lhs, OPERATOR_TYPE op, expression rhs)
			:_lhs(lhs),
			_operator(op),
			_rhs(rhs)
		{
		}
		bool assignment::isPure() const
		{
			return false;
		}
		std::ostream& operator<<(std::ostream& out, assignment const& x)
		{
			out << x._lhs << 
#ifdef TOKEN_ID
				x._operator
#else
				static_cast<operators::EOperators>(x._operator)
#endif
				<< x._rhs; return out;
		}
	}
}