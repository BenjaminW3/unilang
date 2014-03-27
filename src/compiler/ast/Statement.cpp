#include <unilang/compiler/ast/Statement.hpp>

#include <boost/variant/get.hpp>

namespace unilang
{
	namespace ast
	{
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		SStatement::SStatement() : base_type() {}
		SStatement::SStatement(SIfStatement const & val) : base_type(val) {}
		//SStatement::SStatement(SWhileStatement const & val) : base_type(val) {}
		//SStatement::SStatement(SReturnStatement const & const& val) : base_type(val) {}
		SStatement::SStatement(SAssignment const & val) : base_type(val) {}
		SStatement::SStatement(SExpression const & val) : base_type(val) {}
		SStatement::SStatement(SStatementVector const & val) : base_type(val) {}
		
		std::ostream& operator<<(std::ostream& out, SStatement const & x)
		{
			switch(x.get().which())
			{
				case 0: out << boost::get<SIfStatement>(x); break;
				//case 3: out << boost::get<SWhileStatement>(x); break;
				//case 4: out << boost::get<SReturnStatement>(x); break;
				case 1: out << boost::get<SAssignment>(x); break;
				case 2: out << boost::get<SExpression>(x); break;
				case 3: out << boost::get<SStatementVector>(x); break;
				default: out << "undefined statement"; break;
			}
			return out;
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SStatementVector const & x)
		{
			for(SStatement const & st : x)
			{
				out << st << std::endl;
			}
			return out;
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, SIfStatement const & x)
		{
			out << "if(" << x.m_expCondition << ")" << std::endl
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