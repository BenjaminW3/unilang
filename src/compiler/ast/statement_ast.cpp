#include <unilang/compiler/ast/statement_ast.hpp>

#include <unilang/compiler/ast/operators_def.hpp>		// assignment default constructor

#include <boost/variant/get.hpp>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		assignment::assignment() :
			_lhs(/*"unnamed-identifier"*/),
			_uiOperatorID(static_cast<OPERATOR_TYPE>(operators::EOperators::assign)),
			_rhs()
		{
		}
		assignment::assignment(identifier lhs, OPERATOR_TYPE op, expression rhs) :
			_lhs(lhs),
			_uiOperatorID(op),
			_rhs(rhs)
		{
		}
		std::ostream& operator<<(std::ostream& out, assignment const & x)
		{
			out << x._lhs <<
#ifdef TOKEN_ID
				x._uiOperatorID
#else
				static_cast<operators::EOperators>(x._uiOperatorID)
#endif
				<< x._rhs; return out;
		}

		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		statement::statement() : base_type() {}
		statement::statement(if_statement const & val) : base_type(val) {}
		//statement::statement(while_statement const & val) : base_type(val) {}
		//statement::statement(return_statement const & const& val) : base_type(val) {}
		statement::statement(assignment const & val) : base_type(val) {}
		statement::statement(expression const & val) : base_type(val) {}
		statement::statement(statement_vector const & val) : base_type(val) {}
		
		std::ostream& operator<<(std::ostream& out, statement const & x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<if_statement>(x); break;
				//case 3: out << boost::get<while_statement>(x); break;
				//case 4: out << boost::get<return_statement>(x); break;
				case 1: out << boost::get<assignment>(x); break;
				case 2: out << boost::get<expression>(x); break;
				case 3: out << boost::get<statement_vector>(x); break;
				default: out << "undefined statement"; break;
			}
			return out;
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, statement_vector const & x)
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
		std::ostream& operator<<(std::ostream& out, if_statement const & x)
		{
			out << "if(" << x._expCondition << ")" << std::endl
				<< "{" << std::endl 
				<< x._thenStatementList
				<< "}";
			if(x._elseOptionalStatementList.is_initialized())
			{
				out << "else"
					<< "{"
					<< x._elseOptionalStatementList.get()
					<< "}";
			}
			return out;
		}
	}
}