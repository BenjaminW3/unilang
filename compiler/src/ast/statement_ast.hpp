#pragma once

#include "ast_base.hpp"
#include "identifier_ast.hpp"
#include "expression_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/optional.hpp>

#include <ostream>

namespace unilang 
{
	namespace ast
	{
		//#########################################################################
		//! Assignment consists of a identifier and an expression.
		//#########################################################################
		struct assignment :	public ast_base
		{
			identifier lhs;
			token_ids::type operator_;
			expression rhs;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, assignment const& x);

		// predefinitions for variant
		struct if_statement;
		//struct while_statement;
		//struct return_statement;
		struct statement_list;

		//#########################################################################
		//! A statement
		//#########################################################################
		struct statement :	public ast_base,
							boost::spirit::extended_variant<	assignment,
																expression,
																boost::recursive_wrapper<if_statement>,
																//boost::recursive_wrapper<while_statement>,
																//boost::recursive_wrapper<return_statement>,
																boost::recursive_wrapper<statement_list>
								>
		{
			statement();
			statement(assignment val);
			statement(expression val);
			statement(if_statement val);
			//statement(while_statement val);
			//statement(return_statement const& val);
			statement(statement_list const& val);

			inline bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, statement const& x);
		
		//#########################################################################
		//! A statement list of multiple statements.
		//#########################################################################
		struct statement_list :	std::list<statement>,
								public ast_base
		{
			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, statement_list const& x);

		//#########################################################################
		//! If-statement
		//#########################################################################
		struct if_statement :	public ast_base
		{
			expression condition;
			statement_list then;
			boost::optional<statement_list> else_;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, if_statement const& x);

		//#########################################################################
		//! While statement
		//#########################################################################
		/*struct while_statement :	public ast_base
		{
			expression condition;
			statement body;
		};*/

		//#########################################################################
		//! Return statement
		//#########################################################################
		/*struct return_statement :	public ast_base
		{
		};*/
	}
}
BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::assignment,
    (unilang::ast::identifier, lhs)
	(unilang::token_ids::type, operator_)
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