#pragma once

#include "identifier_ast.hpp"
#include "expression_ast.hpp"

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <list>

namespace unilang 
{
	namespace ast
	{
		struct assignment
		{
			identifier lhs;
			expression rhs;
		};
		inline std::ostream& operator<<(std::ostream& out, assignment const& x)
		{
			out << x.lhs << "=" << x.rhs << std::endl; return out;
		}

		struct variable_definition
		{
			boost::optional<bool> constQual;
			identifier type;
			boost::optional<identifier> name;
			//boost::optional<std::list<expression>> rhs;
		};
		inline std::ostream& operator<<(std::ostream& out, variable_definition const& x)
		{
			if(!x.constQual.is_initialized())
			{
				out << "~";
			}
			out << x.type;
			if(x.name.is_initialized())
			{
				out << ":" << x.name.get().name;
			}
			out << "()";
			return out;
		}

		struct if_statement;
		//struct while_statement;
		struct statement_list;
		//struct return_statement;

		typedef boost::variant<
				variable_definition
			  , assignment
			  , boost::recursive_wrapper<if_statement>
		//	  , boost::recursive_wrapper<while_statement>
		//	  , boost::recursive_wrapper<return_statement>
			  , boost::recursive_wrapper<statement_list>
			>
		statement;

		struct statement_list : std::list<statement>
		{
		};
		inline std::ostream& operator<<(std::ostream& out, statement_list const& x)
		{
			BOOST_FOREACH(statement const & st , x)
			{
				out << st << std::endl;
			}
			return out;
		}

		struct if_statement
		{
			expression condition;
			statement_list then;
			boost::optional<statement_list> else_;
		};
		inline std::ostream& operator<<(std::ostream& out, if_statement const& x)
		{
			out << "if(" << x.condition << ")" << std::endl
				<< "{" << std::endl 
				<< x.then
				<< "}";
			if(x.else_.is_initialized())
			{
				out << "else"
					<< "{"
					<< x.else_.get()
					<< "}";
			}
			return out;
		}

		/*struct while_statement
		{
			expression condition;
			statement body;
		};*/

		/*struct return_statement
		{
		};*/

		inline std::ostream& operator<<(std::ostream& out, statement const& x)
		{
			switch(x.which())
			{
				case 0: out << boost::get<variable_definition>(x); break;
				case 1: out << boost::get<assignment>(x); break;
				case 2: out << boost::get<if_statement>(x); break;
				//case 3: out << boost::get<while_statement>(x); break;
				//case 4: out << boost::get<return_statement>(x); break;
				case 3: out << boost::get<statement_list>(x); break;
				default: out << "undefine-expression"; break;
			}
			return out;
		}
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::variable_definition,
	(boost::optional<bool>, constQual)
    (unilang::ast::identifier, type)
    (boost::optional<unilang::ast::identifier>, name)
    //(boost::optional<std::list<unilang::ast::expression>>, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::assignment,
    (unilang::ast::identifier, lhs)
    (unilang::ast::expression, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::if_statement,
    (unilang::ast::expression, condition)
    (unilang::ast::statement_list, then)
    (boost::optional<unilang::ast::statement_list>, else_)
)

/*BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::while_statement,
    (unilang::ast::expression, condition)
    (unilang::ast::statement_list, body)
)*/

/*BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::return_statement,
    (boost::optional<unilang::ast::expression>, expr)
)*/