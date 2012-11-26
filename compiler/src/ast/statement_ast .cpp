#include "statement_ast.hpp"

#include <boost/variant/get.hpp>

namespace unilang 
{
	namespace ast
	{	
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		statement::statement() : base_type() {}
		statement::statement(if_statement const& val) : base_type(val) {}
		//statement::statement(while_statement const& val) : base_type(val) {}
		//statement::statement(return_statement const& const& val) : base_type(val) {}
		statement::statement(expression const& val) : base_type(val) {}
		statement::statement(statement_list const& val) : base_type(val) {}
		
		bool statement::isPure() const
		{
			switch(get().which())
			{
				case 0: return boost::get<ast::if_statement>(*this).isPure(); break;
				//case 3: return boost::get<ast::while_statement>(*this).isPure(); break;
				//case 4: return boost::get<ast::return_statement>(*this).isPure(); break;
				case 1: return boost::get<ast::expression>(*this).isPure(); break;
				case 2: return boost::get<ast::statement_list>(*this).isPure(); break;
				default: throw std::runtime_error("undefine-statement"); break;
			}
		}
		std::ostream& operator<<(std::ostream& out, statement const& x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<if_statement>(x); break;
				//case 3: out << boost::get<while_statement>(x); break;
				//case 4: out << boost::get<return_statement>(x); break;
				case 1: out << boost::get<expression>(x); break;
				case 2: out << boost::get<statement_list>(x); break;
				default: out << "undefine-statement"; break;
			}
			return out;
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool statement_list::isPure() const
		{
			for(ast::statement const & st : *this)
			{
				if(!st.isPure()) {return false;}
			}
			return true;
		}
		std::ostream& operator<<(std::ostream& out, statement_list const& x)
		{
			for(statement const & st : x)
			{
				out << st << std::endl;
			}
			return out;
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		bool if_statement::isPure() const
		{
			return _condition.isPure() && _then.isPure() && (!_else.is_initialized() || _else.get().isPure());
		}
		std::ostream& operator<<(std::ostream& out, if_statement const& x)
		{
			out << "if(" << x._condition << ")" << std::endl
				<< "{" << std::endl 
				<< x._then
				<< "}";
			if(x._else.is_initialized())
			{
				out << "else"
					<< "{"
					<< x._else.get()
					<< "}";
			}
			return out;
		}
	}
}