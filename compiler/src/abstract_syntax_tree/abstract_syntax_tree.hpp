#pragma once

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <list>

namespace unilang 
{
	namespace abstract_syntax_tree
	{
		///////////////////////////////////////////////////////////////////////////
		//  The AST
		///////////////////////////////////////////////////////////////////////////
		struct null {};
		struct unary;
		struct function_call;
		struct expression;

		struct identifier
		{
			std::string name;
		};

		typedef boost::variant<
				null
			  , bool
			  , unsigned int
			  , identifier
			  , boost::recursive_wrapper<unary>
			  , boost::recursive_wrapper<function_call>
			  , boost::recursive_wrapper<expression>
			>
		operand;

		enum optoken
		{
			op_plus,
			op_minus,
			op_times,
			op_divide,
			op_positive,
			op_negative,
			op_not,
			op_equal,
			op_not_equal,
			op_less,
			op_less_equal,
			op_greater,
			op_greater_equal,
			op_and,
			op_or
		};

		struct unary
		{
			optoken operator_;
			operand operand_;
		};

		struct operation
		{
			optoken operator_;
			operand operand_;
		};

		struct function_call
		{
			identifier function_name;
			std::list<expression> args;
		};

		struct expression
		{
			operand first;
			std::list<operation> rest;
		};

		struct assignment
		{
			identifier lhs;
			expression rhs;
		};

		struct variable_definition
		{
			boost::optional<bool> constQual;
			identifier type;
			boost::optional<identifier> name;
			boost::optional<std::list<expression>> rhs;
		};

		struct if_statement;
		struct while_statement;
		struct statement_list;
		//struct return_statement;

		typedef boost::variant<
				variable_definition
			  , assignment
			  , boost::recursive_wrapper<if_statement>
			  , boost::recursive_wrapper<while_statement>
		//	  , boost::recursive_wrapper<return_statement>
			  , boost::recursive_wrapper<statement_list>
			>
		statement;

		struct statement_list : std::list<statement> {};

		struct if_statement
		{
			expression condition;
			statement then;
			boost::optional<statement> else_;
		};

		struct while_statement
		{
			expression condition;
			statement body;
		};

		/*struct return_statement
		{
		};*/

		struct function_declaration
		{
			identifier function_name;
			std::list<variable_definition> arguments;
			std::list<variable_definition> return_values;
		};

		struct function
		{
			function_declaration decl;
			statement_list body;
		};

		// structure/type/object definitions
		typedef boost::variant<
			variable_definition,
			function_declaration,
			function
		> meta_entity;

		struct module
		{
			std::list<meta_entity> metaEntities;
		};

		// print functions for debugging
		inline std::ostream& operator<<(std::ostream& out, null)
		{
			out << "null"; return out;
		}

		inline std::ostream& operator<<(std::ostream& out, identifier const& id)
		{
			out << id.name; return out;
		}
	}
	namespace ast = abstract_syntax_tree;
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::identifier,
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::unary,
    (unilang::ast::optoken, operator_)
    (unilang::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::operation,
    (unilang::ast::optoken, operator_)
    (unilang::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_call,
    (unilang::ast::identifier, function_name)
    (std::list<unilang::ast::expression>, args)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::expression,
    (unilang::ast::operand, first)
    (std::list<unilang::ast::operation>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::variable_definition,
	(boost::optional<bool>, constQual)
    (unilang::ast::identifier, type)
    (boost::optional<unilang::ast::identifier>, name)
    (boost::optional<std::list<unilang::ast::expression>>, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::assignment,
    (unilang::ast::identifier, lhs)
    (unilang::ast::expression, rhs)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::if_statement,
    (unilang::ast::expression, condition)
    (unilang::ast::statement, then)
    (boost::optional<unilang::ast::statement>, else_)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::while_statement,
    (unilang::ast::expression, condition)
    (unilang::ast::statement, body)
)

/*BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::return_statement,
    (boost::optional<unilang::ast::expression>, expr)
)*/

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_declaration,
    (unilang::ast::identifier, function_name)
    (std::list<unilang::ast::variable_definition>, arguments)
    (std::list<unilang::ast::variable_definition>, return_values)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function,
    (unilang::ast::function_declaration, decl)
    (unilang::ast::statement_list, body)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::module,
    (std::list<unilang::ast::meta_entity>, metaEntities)
)