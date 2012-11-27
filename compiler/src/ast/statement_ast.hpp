#pragma once

#include "ast_base.hpp"
#include "identifier_ast.hpp"
#include "expression_ast.hpp"

#include <boost/variant/recursive_variant.hpp>
#include <boost/optional.hpp>

#include <ostream>

namespace unilang 
{
	namespace ast
	{
		// forward declarations for variant
		struct if_statement;
		//struct while_statement;
		//struct return_statement;
		struct statement_list;

		//#########################################################################
		//! A statement.
		//#########################################################################
		struct statement :	public ast_base,
							boost::spirit::extended_variant<	boost::recursive_wrapper<if_statement>,
																//boost::recursive_wrapper<while_statement>,
																//boost::recursive_wrapper<return_statement>,
																expression,
																boost::recursive_wrapper<statement_list>
								>
		{
			statement();
			statement(if_statement const& val);
			//statement(while_statement const& val);
			//statement(return_statement const& val);
			statement(expression const& val);
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
		//! If-statement.
		//#########################################################################
		struct if_statement :	public ast_base
		{
			expression _condition;
			statement_list _then;
			boost::optional<statement_list> _else;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, if_statement const& x);

		//#########################################################################
		//! While statement.
		//#########################################################################
		/*struct while_statement :	public ast_base
		{
			expression _condition;
			statement _body;
		};*/

		//#########################################################################
		//! Return statement.
		//#########################################################################
		/*struct return_statement :	public ast_base
		{
		};*/
	}
}