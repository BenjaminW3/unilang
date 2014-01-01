#pragma once

#include <unilang/compiler/ast/ast_base.hpp>
#include <unilang/compiler/ast/identifier_ast.hpp>
#include <unilang/compiler/ast/expression_ast.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/optional.hpp>

#include <vector>
#include <ostream>

namespace unilang
{
	namespace ast
	{
		// forward declarations for variant
		struct if_statement;
		//struct while_statement;
		//struct return_statement;
		struct statement_vector;

		//#########################################################################
		//! A statement.
		//#########################################################################
		struct statement :
			public ast_base,
			boost::spirit::extended_variant<	boost::recursive_wrapper<if_statement>,
												//boost::recursive_wrapper<while_statement>,
												//boost::recursive_wrapper<return_statement>,
												assignment,
												expression,
												boost::recursive_wrapper<statement_vector>
											>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			statement();
			statement(if_statement const& val);
			//statement(while_statement const& val);
			//statement(return_statement const& val);
			statement(assignment const& val);
			statement(expression const& val);
			statement(statement_vector const& val);

			inline bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, statement const& x);
		
		//#########################################################################
		//! A statement list of multiple statements.
		//#########################################################################
		struct statement_vector :
			std::vector<statement>,
			public ast_base
		{
			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, statement_vector const& x);

		//#########################################################################
		//! If-statement.
		//#########################################################################
		struct if_statement :
			public ast_base
		{
			expression _expCondition;
			statement_vector _thenStatementList;
			boost::optional<statement_vector> _elseOptionalStatementList;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, if_statement const& x);

		//#########################################################################
		//! While statement.
		//#########################################################################
		/*struct while_statement :
			public ast_base
		{
			expression _expCondition;
			statement _body;
		};*/

		//#########################################################################
		//! Return statement.
		//#########################################################################
		/*struct return_statement :
			public ast_base
		{
		};*/
	}
}