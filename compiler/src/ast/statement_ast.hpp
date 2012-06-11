#pragma once

#include "identifier_ast.hpp"
#include "expression_ast.hpp"

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <list>

namespace unilang 
{
	namespace ast
	{
		//#########################################################################
		//! Assignment consists of a identifier and an expression.
		//#########################################################################
		struct assignment
		{
			identifier lhs;
			expression rhs;
		};
		inline std::ostream& operator<<(std::ostream& out, assignment const& x)
		{
			out << x.lhs << "=" << x.rhs << std::endl; return out;
		}

		// predefinitions for variant
		struct if_statement;
		//struct while_statement;
		//struct return_statement;
		struct statement_list;

		//#########################################################################
		//! A statement could be different things.
		//#########################################################################
		typedef boost::variant<
				assignment
			  , expression
			  , boost::recursive_wrapper<if_statement>
		//	  , boost::recursive_wrapper<while_statement>
		//	  , boost::recursive_wrapper<return_statement>
			  , boost::recursive_wrapper<statement_list>
			>
		statement;
		// predefinition
		std::ostream& operator<<(std::ostream& out, statement const& x);

		//#########################################################################
		//! A statement list of multiple statements.
		//#########################################################################
		struct statement_list : std::list<statement>
		{
		};
		inline std::ostream& operator<<(std::ostream& out, statement_list const& x)
		{
			for(statement const & st : x)
			{
				out << st << std::endl;
			}
			return out;
		}

		//#########################################################################
		//! If-statement
		//#########################################################################
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

		//#########################################################################
		//! While statement
		//#########################################################################
		/*struct while_statement
		{
			expression condition;
			statement body;
		};*/

		//#########################################################################
		//! Return statement
		//#########################################################################
		/*struct return_statement
		{
		};*/

		inline std::ostream& operator<<(std::ostream& out, statement const& x)
		{
			switch(x.which())
			{
				case 0: out << boost::get<assignment>(x); break;
				case 1: out << boost::get<expression>(x); break;
				case 2: out << boost::get<if_statement>(x); break;
				//case 3: out << boost::get<while_statement>(x); break;
				//case 4: out << boost::get<return_statement>(x); break;
				case 3: out << boost::get<statement_list>(x); break;
				default: out << "undefine-statement"; break;
			}
			return out;
		}
	}
}
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