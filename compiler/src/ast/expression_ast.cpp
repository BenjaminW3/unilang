#include "expression_ast.hpp"

#include "operators_def.hpp"		// assignment default constructor

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
				default: throw std::runtime_error("undefined primary_expr"); break;
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
				default: out << "undefined primary_expr"; break;
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
		//operand::operand(assignment const& val) : base_type(val) {}
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
				default: throw std::runtime_error("undefined operand"); break;
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
				//case 4: out << boost::get<assignment>(x); break;
				default: out << "undefined operand"; break;
			}
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool unary_expr::isPure() const
		{
			return _opOperand.isPure();
		}
		std::ostream& operator<<(std::ostream& out, unary_expr const& x)
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
		bool operation::isPure() const
		{
			return _opOperand.isPure();
		}
		std::ostream& operator<<(std::ostream& out, operation const& x)
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
		bool expression::isPure() const
		{
			if(!_firstOperand.isPure()){return false;}
			// all rest have to be pure
			for(ast::operation const & op : _vRestOperands)
			{
				if(!op.isPure()) {return false;}
			}
			return true;
		}
		std::ostream& operator<<(std::ostream& out, expression const& x)
		{
			out << x._firstOperand; 
			for(operation const & op : x._vRestOperands)
			{
				out << op;
			}
			return out;
		}
		std::ostream& operator<<(std::ostream& out, std::vector<expression> const& x)
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
		bool function_call::isPure() const
		{
			// FIXME: pure test for function itself

			// all parameters have to be pure
			for(ast::expression const & ex : _vArgumentExpressions)
			{
				if(!ex.isPure()) {return false;}
			}

			return true;
		}
		std::ostream& operator<<(std::ostream& out, function_call const& x)
		{
			out << x._idfName << "(" << x._vArgumentExpressions << ")";
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		type_declaration::type_declaration()
			:_bHasMutableQualifier(false),
			_idfName(/*"unnamed-type"*/)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		type_declaration::type_declaration(identifier const & type_identifier)
			:_bHasMutableQualifier(false),
			_idfName(type_identifier)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		type_declaration::type_declaration(bool bHasMutableQualifier, identifier const & type_identifier)
			:_bHasMutableQualifier(bHasMutableQualifier),
			_idfName(type_identifier)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool type_declaration::isPure() const
		{
			// it has to be non mutable
			return (!_bHasMutableQualifier);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string type_declaration::build_mangled_name() const
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
		std::ostream& operator<<(std::ostream& out, type_declaration const& x)
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
			for(ast::expression const & ex : _vParameterExpressions)
			{
				if(!ex.isPure()) {return false;}
			}
			// it has to be non mutable
			return _declaration.isPure();
		}
		std::ostream& operator<<(std::ostream& out, variable_definition const& x)
		{
			out << x._declaration << "{" << x._vParameterExpressions << "}";
			return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		assignment::assignment()
			:_lhs(/*"unnamed-identifier"*/),
			_uiOperatorID(static_cast<OPERATOR_TYPE>(operators::EOperators::assign)),
			_rhs()
		{
		}
		assignment::assignment(identifier lhs, OPERATOR_TYPE op, expression rhs)
			:_lhs(lhs),
			_uiOperatorID(op),
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
				x._uiOperatorID
#else
				static_cast<operators::EOperators>(x._uiOperatorID)
#endif
				<< x._rhs; return out;
		}
	}
}