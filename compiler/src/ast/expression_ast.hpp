#pragma once

#include "identifier_ast.hpp"

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/get.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/foreach.hpp>
#include <list>

namespace unilang 
{
	namespace ast
	{
		///////////////////////////////////////////////////////////////////////////
		//  The AST
		///////////////////////////////////////////////////////////////////////////
		struct unaryOp;
		struct binaryOp;
		struct function_call;

		typedef boost::variant<
				unsigned int
			  , bool
			  , identifier
			  , boost::recursive_wrapper<unaryOp>
			  , boost::recursive_wrapper<binaryOp>
			  , boost::recursive_wrapper<function_call>
			>
		expression;

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

		struct unaryOp
		{
			optoken operator_;
			expression operand;
		};
		inline std::ostream& operator<<(std::ostream& out, unaryOp const& x)
		{
			out << x.operator_ << "(" << x.operand << ")"; return out;
		}

		struct binaryOp
		{
			expression operand1;
			optoken operator_;
			expression operand2;
		};
		inline std::ostream& operator<<(std::ostream& out, binaryOp const& x)
		{
			out << "(" << x.operand1 << ")" << x.operator_ << "(" << x.operand2 << ")"; return out;
		}

		struct function_call
		{
			identifier idf;
			std::list<expression> arguments;
		};
		inline std::ostream& operator<<(std::ostream& out, function_call const& x)
		{
			out << x.idf << "(";
			bool bFirst = true;
			BOOST_FOREACH(expression const & ex , x.arguments)
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

		inline std::ostream& operator<<(std::ostream& out, expression const& x)
		{
			switch(x.which())
			{
				case 0: out << boost::get<unsigned int>(x); break;
				case 1: out << boost::get<bool>(x); break;
				case 2: out << boost::get<identifier>(x); break;
				case 3: out << boost::get<unaryOp>(x); break;
				case 4: out << boost::get<binaryOp>(x); break;
				case 5: out << boost::get<function_call>(x); break;
				default: out << "undefine-expression"; break;
			}
			return out;
		}
	}
}

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::unaryOp,
    (unilang::ast::optoken, operator_)
    (unilang::ast::expression, operand)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::binaryOp,
    (unilang::ast::expression, operand1)
    (unilang::ast::optoken, operator_)
    (unilang::ast::expression, operand2)
)

BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::function_call,
    (unilang::ast::identifier, idf)
    (std::list<unilang::ast::expression>, arguments)
)

/*BOOST_FUSION_ADAPT_STRUCT(
    unilang::ast::expression_types,
    (unilang::ast::operand, first)
    (std::vector<unilang::ast::operation>, rest)
)*/