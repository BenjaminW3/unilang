#include "expression_ast.hpp"

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
		primary_expr::primary_expr(unsigned int val) : base_type(val) {}
		primary_expr::primary_expr(int val) : base_type(val) {}
		primary_expr::primary_expr(bool val) : base_type(val) {}
		primary_expr::primary_expr(identifier const& val) : base_type(val) {}
		primary_expr::primary_expr(variable_definition const& val) : base_type(val) {}
		primary_expr::primary_expr(expression const& val) : base_type(val) {}
		primary_expr::primary_expr(primary_expr const& rhs) : base_type(rhs.get()) {}

		bool primary_expr::isPure() const
		{
			switch(get().which())
			{
				case 0: return true; break;
				case 1: return true; break;
				case 2: return true; break;
				case 3: return true; break;
				case 4: return boost::get<ast::identifier>(*this).isPure(); break;
				case 5: return boost::get<ast::variable_definition>(*this).isPure(); break;
				case 6: return boost::get<ast::expression>(*this).isPure(); break;
				default: throw std::runtime_error("undefined-primary_expr"); break;
			}
		}
		std::ostream& operator<<(std::ostream& out, primary_expr const& x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<long double>(x); break;
				case 1: out << boost::get<unsigned int>(x); break;
				case 2: out << boost::get<int>(x); break;
				case 3: out << boost::get<bool>(x); break;
				case 4: out << boost::get<identifier>(x); break;
				case 5: out << boost::get<variable_definition>(x); break;
				case 6: out << boost::get<expression>(x); break;
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
		operand::operand(operand const& rhs): base_type(rhs.get()) {}

		bool operand::isPure() const
		{
			switch(get().which())
			{
				case 0: return boost::get<primary_expr>(*this).isPure(); break;
				case 1: return boost::get<unary_expr>(*this).isPure(); break;
				case 2: return boost::get<function_call>(*this).isPure(); break;
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
				default: out << "undefined-operand"; break;
			}
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool unary_expr::isPure() const
		{
			return operand_.isPure();
		}
		std::ostream& operator<<(std::ostream& out, unary_expr const& x)
		{
			out << x.operator_ << x.operand_; return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool operation::isPure() const
		{
			return operand_.isPure();
		}
		std::ostream& operator<<(std::ostream& out, operation const& x)
		{
			out << x.operator_;
			out << x.operand_;
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool expression::isPure() const
		{
			if(!first.isPure()){return false;}
			// all rest have to be pure
			for(ast::operation const & op : rest)
			{
				if(!op.isPure()) {return false;}
			}
			return true;
		}
		std::ostream& operator<<(std::ostream& out, expression const& x)
		{
			out << x.first; 
			for(operation const & op : x.rest)
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
			for(ast::expression const & ex : arguments)
			{
				if(!ex.isPure()) {return false;}
			}

			return true;
		}
		std::ostream& operator<<(std::ostream& out, function_call const& x)
		{
			out << x.idf << "(";
			bool bFirst = true;
			for(expression const & ex : x.arguments)
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
		bool type_declaration::isPure() const
		{
			// it has to be non mutable
			return (!mutableQualifier);
		}
		std::ostream& operator<<(std::ostream& out, type_declaration const& x)
		{
			if(x.mutableQualifier)
			{
				out << "~";
			}
			out << x.type_identifier;
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool variable_declaration::isPure() const
		{
			// it has to be non mutable
			return type.isPure();
		}
		std::ostream& operator<<(std::ostream& out, variable_declaration const& x)
		{
			out << x.type;
			if(x.name.is_initialized())
			{
				out << ":" << x.name.get().name;
			}
			return out;
		}
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool variable_definition::isPure() const
		{
			// FIXME: constructor has to be pure!

			// all parameters have to be pure
			for(ast::expression const & ex : parameters)
			{
				if(!ex.isPure()) {return false;}
			}
			// it has to be non mutable
			return decl.isPure();
		}
		std::ostream& operator<<(std::ostream& out, variable_definition const& x)
		{
			out << x.decl;
			for(expression const & ex : x.parameters)
			{
				out << ex;
			}
			out << ")";
			return out;
		}
	}
}