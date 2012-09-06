#pragma once

#include "../lexer/ids.hpp"
#include "ast_base.hpp"
#include "identifier_ast.hpp"

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>
#include <boost/optional.hpp>

#include <ostream>
#include <list>

namespace unilang 
{
	namespace ast
	{
		// predefinitions
		struct variable_definition;
		struct expression;

		//#########################################################################
		//! A primary expression
		//#########################################################################
		struct primary_expr :	public ast_base,
								boost::spirit::extended_variant<	long double,
																	unsigned int,
																	int,
																	bool,
								  									identifier,
																	boost::recursive_wrapper<variable_definition>,
																	boost::recursive_wrapper<expression>
								>
		{
			primary_expr();
			primary_expr(long double val);
			primary_expr(unsigned int val);
			primary_expr(int val);
			primary_expr(bool val);
			primary_expr(identifier const& val);
			primary_expr(variable_definition const& val);
			primary_expr(expression const& val);
			primary_expr(primary_expr const& rhs);

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, primary_expr const& x);

		// predefinitions
		struct unary_expr;
		struct function_call;
		//#########################################################################
		//! A operand
		//#########################################################################
		struct operand :	public ast_base,
							boost::spirit::extended_variant<	primary_expr,
																boost::recursive_wrapper<unary_expr>,
																boost::recursive_wrapper<function_call>
							>
		{
			operand();
			operand(primary_expr const& val);
			operand(unary_expr const& val);
			operand(function_call const& val);
			operand(operand const& rhs);

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, operand const& x);
		//#########################################################################
		//! A unary operation
		//#########################################################################
		struct unary_expr :	public ast_base
		{
			token_ids::type operator_;
			operand operand_;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, unary_expr const& x);
		//#########################################################################
		//! ??? unary_expr
		//#########################################################################
		struct operation :	public ast_base
		{
			token_ids::type operator_;
			operand operand_;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, operation const& x);
		//#########################################################################
		//! An expression
		//#########################################################################
		struct expression :	public ast_base
		{
			operand first;
			std::list<operation> rest;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, expression const& x);
		//#########################################################################
		//! A function call.
		//#########################################################################
		struct function_call :	public ast_base
		{
			identifier idf;
			std::list<expression> arguments;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_call const& x);
		//#########################################################################
		//! A type declaration.
		//#########################################################################
		struct type_declaration :	public ast_base
		{
			bool mutableQualifier;
			identifier type_identifier;
			
			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, type_declaration const& x);
		//#########################################################################
		//! A variable declaration.
		//#########################################################################
		struct variable_declaration :	public ast_base
		{
			type_declaration type;
			boost::optional<identifier> name;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, variable_declaration const& x);
		//#########################################################################
		//! A variable definition.
		//#########################################################################
		struct variable_definition :	public ast_base
		{
			variable_declaration decl;
			std::list<expression> parameters;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, variable_definition const& x);
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_call,
    (unilang::ast::identifier, idf)
    (std::list<unilang::ast::expression>, arguments)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::unary_expr,
    (unilang::token_ids::type, operator_)
    (unilang::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::operation,
    (unilang::token_ids::type, operator_)
    (unilang::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::expression,
    (unilang::ast::operand, first)
    (std::list<unilang::ast::operation>, rest)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::type_declaration,
	(bool, mutableQualifier)
    (unilang::ast::identifier, type_identifier)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::variable_declaration,
    (unilang::ast::type_declaration, type)
    (boost::optional<unilang::ast::identifier>, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::variable_definition,
    (unilang::ast::variable_declaration, decl)
    (std::list<unilang::ast::expression>, parameters)
)