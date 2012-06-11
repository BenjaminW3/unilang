#pragma once

#include "ast_base.hpp"
#include "identifier_ast.hpp"

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
		// predefinitions
		struct function_call;
		struct unaryOp;
		struct expression;
		struct variable_definition;

		//#########################################################################
		//! A operand
		//#########################################################################
		typedef boost::variant<		unsigned int
								  ,	bool
								  , boost::recursive_wrapper<function_call>
								  , identifier
								  , boost::recursive_wrapper<unaryOp>
								  , boost::recursive_wrapper<expression>
								  , boost::recursive_wrapper<variable_definition>
							>operand;
		std::ostream& operator<<(std::ostream& out, operand const& x);

		//#########################################################################
		//! Optokens
		//#########################################################################
		enum optoken
		{
			op_plus,
			op_minus,
			op_times,
			op_divide,
			op_positive,
			op_negative,
			op_not,
			op_stringify,
			op_equal,
			op_not_equal,
			op_less,
			op_less_equal,
			op_greater,
			op_greater_equal,
			op_and,
			op_or
		};
		inline std::ostream& operator<<(std::ostream& out, optoken const& x)
		{
			switch(x)
			{
				case op_plus: out << "op_plus"; break;
				case op_minus: out << "op_minus"; break;
				case op_times: out << "op_times"; break;
				case op_divide: out << "op_divide"; break;
				case op_positive: out << "op_positive"; break;
				case op_negative: out << "op_negative"; break;
				case op_not: out << "op_not"; break;
				case op_stringify: out << "op_stringify"; break;
				case op_equal: out << "op_equal"; break;
				case op_not_equal: out << "op_not_equal"; break;
				case op_less: out << "op_less"; break;
				case op_less_equal: out << "op_less_equal"; break;
				case op_greater: out << "op_greater"; break;
				case op_greater_equal: out << "op_greater_equal"; break;
				case op_and: out << "op_and"; break;
				case op_or: out << "op_or"; break;
				default: out << "unknown-operation"; break;
			}
			return out;
		}

		//#########################################################################
		//! A unary operation
		//#########################################################################
		struct unaryOp : public ast_base
		{
			optoken operator_;
			operand operand_;
		};
		inline std::ostream& operator<<(std::ostream& out, unaryOp const& x)
		{
			out << x.operator_ << "(" << x.operand_ << ")"; return out;
		}
		
		//#########################################################################
		//! ??? unaryOp
		//#########################################################################
		struct operation : public ast_base
		{
			optoken operator_;
			operand operand_;
		};
		inline std::ostream& operator<<(std::ostream& out, operation const& x)
		{
			out << x.operator_;
			out << x.operand_;
			return out;
		}
		//#########################################################################
		//! An expression
		//#########################################################################
		struct expression : public ast_base
		{
			operand first;
			std::list<operation> rest;
		};
		inline std::ostream& operator<<(std::ostream& out, expression const& x)
		{
			out << x.first; 
			for(operation const & op : x.rest)
			{
				out << op;
			}
			return out;
		}

		//#########################################################################
		//! A function call.
		//#########################################################################
		struct function_call : public ast_base
		{
			identifier idf;
			std::list<expression> arguments;
		};
		inline std::ostream& operator<<(std::ostream& out, function_call const& x)
		{
			out << x.idf << "(";
			bool bFirst = true;
			for(expression const & ex : x.arguments)
			{
				if(!bFirst)
				{
					out << ", ";
					bFirst = false;
				}
				out << ex;
			}
			out << ")";
			return out;
		}
		//#########################################################################
		//! A type declaration.
		//#########################################################################
		struct type_declaration : public ast_base
		{
			bool mutableQualifier;
			identifier type_name;
		};
		inline std::ostream& operator<<(std::ostream& out, type_declaration const& x)
		{
			if(x.mutableQualifier)
			{
				out << "~";
			}
			out << x.type_name;
			return out;
		}
		
		//#########################################################################
		//! A variable definition.
		//#########################################################################
		struct variable_definition : public ast_base
		{
			type_declaration type;
			boost::optional<identifier> name;
			boost::optional<std::list<expression>> parameters;
		};
		inline std::ostream& operator<<(std::ostream& out, variable_definition const& x)
		{
			out << x.type;
			if(x.name.is_initialized())
			{
				out << x.name.get().name;
			}
			out << "(";
			if(x.parameters.is_initialized())
			{
				for(expression const & ex : x.parameters.get())
				{
					out << ex;
				}
			}
			out << ")";
			return out;
		}

		inline std::ostream& operator<<(std::ostream& out, operand const& x)
		{
			switch(x.which())
			{
				case 0: out << boost::get<unsigned int>(x); break;
				case 1: out << boost::get<bool>(x); break;
				case 2: out << boost::get<function_call>(x); break;
				case 3: out << boost::get<identifier>(x); break;
				case 4: out << boost::get<unaryOp>(x); break;
				case 5: out << boost::get<expression>(x); break;
				case 6: out << boost::get<variable_definition>(x); break;
				default: out << "undefined-expression"; break;
			}
			return out;
		}
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_call,
    (unilang::ast::identifier, idf)
    (std::list<unilang::ast::expression>, arguments)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::unaryOp,
    (unilang::ast::optoken, operator_)
    (unilang::ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::operation,
    (unilang::ast::optoken, operator_)
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
    (unilang::ast::identifier, type_name)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::variable_definition,
    (unilang::ast::type_declaration, type)
    (boost::optional<unilang::ast::identifier>, name)
    (boost::optional<std::list<unilang::ast::expression>>, parameters)
)