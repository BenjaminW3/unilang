#pragma once

#include "operators.hpp"
#include "ast_base.hpp"
#include "identifier_ast.hpp"

#include <boost/variant/recursive_variant.hpp>
#include <boost/spirit/include/support_extended_variant.hpp>
#include <boost/optional.hpp>

#include <ostream>
#include <vector>

#ifdef TOKEN_ID
#define OPERATOR_TYPE operators::EOperators
#else
#define OPERATOR_TYPE size_t
#endif

namespace unilang
{
	namespace ast
	{
		// forward declarations
		struct expression;
		//#########################################################################
		//! A primary expression.
		//#########################################################################
		struct primary_expr :	public ast_base,
								boost::spirit::extended_variant<	long double,
																	uint64_t,
																	bool,
																	identifier,
																	boost::recursive_wrapper<expression>
								>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			primary_expr();
			primary_expr(long double val);
			primary_expr(uint64_t val);
			primary_expr(bool val);
			primary_expr(identifier const& val);
			primary_expr(expression const& val);
			primary_expr(primary_expr const& rhs);

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, primary_expr const& x);

		// forward declarations
		struct unary_expr;
		struct function_call;
		struct variable_definition;
		struct assignment;
		//#########################################################################
		//! A operand.
		//#########################################################################
		struct operand :	public ast_base,
							boost::spirit::extended_variant<	primary_expr,
																boost::recursive_wrapper<unary_expr>,
																boost::recursive_wrapper<function_call>,
																boost::recursive_wrapper<variable_definition>//,
																//boost::recursive_wrapper<assignment>
							>
		{
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			operand();
			operand(primary_expr const& val);
			operand(unary_expr const& val);
			operand(function_call const& val);
			operand(variable_definition const& val);
			//operand(assignment const& val);
			operand(operand const& rhs);

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, operand const& x);

		//#########################################################################
		//! A unary expression.
		//#########################################################################
		struct unary_expr :	public ast_base
		{
			OPERATOR_TYPE _uiOperatorID;
			operand _opOperand;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, unary_expr const& x);

		//#########################################################################
		//! A operation.
		//#########################################################################
		struct operation :	public ast_base
		{
			OPERATOR_TYPE _uiOperatorID;
			operand _opOperand;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, operation const& x);

		//#########################################################################
		//! An expression.
		//#########################################################################
		struct expression :	public ast_base
		{
			operand _firstOperand;
			std::vector<operation> _vRestOperands;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, expression const& x);
		std::ostream& operator<<(std::ostream& out, std::vector<expression> const& x);

		//#########################################################################
		//! A function call.
		//#########################################################################
		struct function_call :	public ast_base
		{
			namespaced_identifier _idfName;
			std::vector<expression> _vArgumentExpressions;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, function_call const& x);

		//#########################################################################
		//! A type declaration.
		//#########################################################################
		struct type_declaration :	public ast_base
		{
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			type_declaration();
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			type_declaration(identifier const & type_identifier);
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			type_declaration(bool bHasMutableQualifier, identifier const & type_identifier);

			//-------------------------------------------------------------------------
			//! 
			//-------------------------------------------------------------------------
			bool isPure() const override;
			//-------------------------------------------------------------------------
			//! \return Unique names for different types.
			//-------------------------------------------------------------------------
			std::string build_mangled_name() const;

			bool _bHasMutableQualifier;
			identifier _idfName;
		};
		std::ostream& operator<<(std::ostream& out, type_declaration const& x);

		//#########################################################################
		//! A variable declaration.
		//#########################################################################
		struct variable_declaration :	public ast_base
		{
			boost::optional<identifier> _optionalIdentifier;
			type_declaration _type;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, variable_declaration const& x);

		//#########################################################################
		//! A variable definition.
		//#########################################################################
		struct variable_definition :	public ast_base
		{
			variable_declaration _declaration;
			std::vector<expression> _vParameterExpressions;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, variable_definition const& x);

		//#########################################################################
		//! Assignment consists of a identifier, the operator and an expression.
		//#########################################################################
		struct assignment :	public ast_base
		{
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			assignment();
			//-------------------------------------------------------------------------
			//! Constructor!
			//-------------------------------------------------------------------------
			assignment(identifier lhs, OPERATOR_TYPE op, expression rhs);

			identifier _lhs;
			OPERATOR_TYPE _uiOperatorID;
			expression _rhs;

			bool isPure() const override;
		};
		std::ostream& operator<<(std::ostream& out, assignment const& x);
	}
}