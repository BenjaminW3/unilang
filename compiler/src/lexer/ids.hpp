#pragma once

#include <ostream>

namespace unilang
{
	struct op
	{
		//#########################################################################
		//! Enumeration of all available types of operations.
		//#########################################################################
		enum EOperationTypes
		{
			binaryOperation			= 0x20000,
			unaryOperation			= 0x40000,
			assignmentOperation		= 0x80000
		};

		//#########################################################################
		//! Enumeration of all available operations.
		//#########################################################################
		enum types
		{
			// binary
			equal               = EOperationTypes::binaryOperation + 1,
			not_equal,
			less,
			less_equal,
			greater,
			greater_equal,

			logical_or,
			logical_and,

			bit_or,
			bit_xor,
			bit_and,
			shift_left,
			shift_right,
			times,
			divide,
			mod,

			plus				= EOperationTypes::unaryOperation | EOperationTypes::binaryOperation + 1,
			minus,

			// unary
			not_				= EOperationTypes::unaryOperation + 1,
			stringify,
			plus_plus,
			minus_minus,

			assign				= EOperationTypes::assignmentOperation + 1,
			bit_or_assign		= EOperationTypes::assignmentOperation + bit_or,
			bit_xor_assign,
			bit_and_assign,
			shift_left_assign,
			shift_right_assign,
			times_assign,
			divide_assign,
			mod_assign,
			plus_assign			= EOperationTypes::unaryOperation | EOperationTypes::binaryOperation | EOperationTypes::assignmentOperation + 1,
			minus_assign,
		};

		static size_t getPrecedenceOfOperator(op::types op);
	};

	
	//-----------------------------------------------------------------------------
	//! \return The precedence of the given operator.
	//! 
	//! // precedence 1
	//! op_comma
	//! 
	//! // precedence 2
	//! op_assign, op_plus_assign, op_minus_assign, op_times_assign, op_divide_assign, op_mod_assign, op_bit_and_assign, op_bit_xor_assign, op_bitor_assign, op_shift_left_assign, op_shift_right_assign
	//! 
	//! // precedence 3
	//! op_logical_or
	//! 
	//! // precedence 4
	//! op_logical_and
	//! 
	//! // precedence 5
	//! op_bit_or
	//! 
	//! // precedence 6
	//! op_bit_xor
	//! 
	//! // precedence 7
	//! op_bit_and
	//! 
	//! // precedence 8
	//! op_equal, op_not_equal
	//! 
	//! // precedence 9
	//! op_less, op_less_equal, op_greater, op_greater_equal
	//! 
	//! // precedence 10
	//! op_shift_left, op_shift_right
	//! 
	//! // precedence 11
	//! op_plus, op_minus
	//! 
	//! // precedence 12
	//! op_times, op_divide, op_mod
	//-----------------------------------------------------------------------------
	inline size_t op::getPrecedenceOfOperator(op::types op)
	{
		switch(op)
		{
			case op::types::equal:				return 8; break;
			case op::types::not_equal:			return 8; break;
			case op::types::less:				return 9; break;
			case op::types::less_equal:			return 9; break;
			case op::types::greater:			return 9; break;
			case op::types::greater_equal:		return 9; break;

			case op::types::logical_or:			return 3; break;
			case op::types::logical_and:		return 4; break;

			case op::types::bit_or:				return 5; break;
			case op::types::bit_xor:			return 6; break;
			case op::types::bit_and:			return 7; break;
			case op::types::shift_left:			return 10; break;
			case op::types::shift_right:		return 10; break;
			case op::types::plus:				return 11; break;
			case op::types::minus:				return 11; break;
			case op::types::times:				return 12; break;
			case op::types::divide:				return 12; break;
			case op::types::mod:				return 12; break;

			// unary operators do not have precedence
			//case op::types::not_: out << "!"; break;
			//case op::types::stringify: out << "$"; break;
			//case op::types::plus_plus: out << "++"; break;
			//case op::types::minus_minus: out << "--"; break;

			case op::types::assign:				return 2; break;
			case op::types::bit_or_assign:		return 2; break;
			case op::types::bit_xor_assign:		return 2; break;
			case op::types::bit_and_assign:		return 2; break;
			case op::types::shift_left_assign:	return 2; break;
			case op::types::shift_right_assign:	return 2; break;
			case op::types::times_assign:		return 2; break;
			case op::types::divide_assign:		return 2; break;
			case op::types::mod_assign:			return 2; break;
			case op::types::plus_assign:		return 2; break;
			case op::types::minus_assign:		return 2; break;

			default: break;
		}
		return 0;
	}

	inline std::ostream& operator<<(std::ostream& out, op::types const& x)
	{
		switch(x)
		{
			case op::types::equal:				out << "=="; break;
			case op::types::not_equal:			out << "!="; break;
			case op::types::less:				out << "<"; break;
			case op::types::less_equal:			out << "<="; break;
			case op::types::greater:			out << ">"; break;
			case op::types::greater_equal:		out << ">="; break;

			case op::types::logical_or:			out << "||"; break;
			case op::types::logical_and:		out << "&&"; break;

			case op::types::bit_or:				out << "|"; break;
			case op::types::bit_xor:			out << "~"; break;
			case op::types::bit_and:			out << "&"; break;
			case op::types::shift_left:			out << "<<"; break;
			case op::types::shift_right:		out << ">>"; break;
			case op::types::plus:				out << "+"; break;
			case op::types::minus:				out << "-"; break;
			case op::types::times:				out << "*"; break;
			case op::types::divide:				out << "/"; break;
			case op::types::mod:				out << "%"; break;

			case op::types::not_:				out << "!"; break;
			case op::types::stringify:			out << "$"; break;
			case op::types::plus_plus:			out << "++"; break;
			case op::types::minus_minus:		out << "--"; break;

			case op::types::assign:				out << "="; break;
			case op::types::bit_or_assign:		out << "|="; break;
			case op::types::bit_xor_assign:		out << "~="; break;
			case op::types::bit_and_assign:		out << "&="; break;
			case op::types::shift_left_assign:	out << "<<="; break;
			case op::types::shift_right_assign:	out << ">>="; break;
			case op::types::times_assign:		out << "*="; break;
			case op::types::divide_assign:		out << "/="; break;
			case op::types::mod_assign:			out << "%="; break;
			case op::types::plus_assign:		out << "+="; break;
			case op::types::minus_assign:		out << "-="; break;

			default: out << "unknown-operation"; break;
		}
		return out;
	}

	struct STokens
	{
		//#########################################################################
		//! Enumeration of all available token IDs.
		//! For all operations they match exactly the values from op::types.
		//#########################################################################
		enum ETokenIDs
		{
			// pseudo tags
			invalid             = -1,

			// misc tags
			identifier          = 1,
			comment,
			whitespace,
			comma,
			colon,
			semicolon,
			tilde,
			question_mark,
			opening_parenthesis,
			closing_parenthesis,
			opening_brace,
			closing_brace,
			//opening_bracket,
			//closing_bracket,
			arrow,
			if_,
			else_,
			//while_,
			//return_,

			equal               = op::types::equal,	// from here on the the values should match op_type
			not_equal,
			less,
			less_equal,
			greater,
			greater_equal,

			logical_or,
			logical_and,

			bit_or,
			bit_xor,
			bit_and,
			shift_left,
			shift_right,
			times,
			divide,
			mod,

			// binary / unary operators with common tokens '+' and '-' can be binary or unary (the lexer cannot distinguish which)
			plus                = op::types::plus,
			minus,

			// unary operators
			not_                = op::types::not_,
			stringify,

			// unary operators with overlaps '++' and '--' can be prefix or postfix (the lexer cannot distinguish which)
			plus_plus,
			minus_minus,

			// assignment operators
			assign              = op::types::assign,
			bit_or_assign       = op::types::bit_or_assign,
			bit_xor_assign,
			bit_and_assign,
			shift_left_assign,
			shift_right_assign,
			times_assign,
			divide_assign,
			mod_assign,
			plus_assign			= op::types::plus_assign,
			minus_assign,

			lit_float			= minus_assign+1,
			lit_uint,
			lit_int,
			lit_boolean,
		};
	};

	inline std::ostream& operator<<(std::ostream& out, STokens::ETokenIDs const& x)
	{
		switch(x)
		{
			case STokens::ETokenIDs::identifier:			out << "identifier"; break;
			case STokens::ETokenIDs::comment:				out << "comment"; break;
			case STokens::ETokenIDs::whitespace:			out << "whitespace"; break;
			case STokens::ETokenIDs::comma:					out << ","; break;
			case STokens::ETokenIDs::colon:					out << ":"; break;
			case STokens::ETokenIDs::semicolon:				out << ";"; break;
			case STokens::ETokenIDs::tilde:					out << "~"; break;
			case STokens::ETokenIDs::question_mark:			out << "?"; break;
			case STokens::ETokenIDs::opening_parenthesis:	out << "("; break;
			case STokens::ETokenIDs::closing_parenthesis:	out << ")"; break;
			case STokens::ETokenIDs::opening_brace:			out << "{"; break;
			case STokens::ETokenIDs::closing_brace:			out << "}"; break;
			//case STokens::ETokenIDs::opening_bracket:		out << "["; break;
			//case STokens::ETokenIDs::closing_bracket:		out << "]"; break;
			case STokens::ETokenIDs::arrow:					out << "->"; break;
			case STokens::ETokenIDs::if_:					out << "if"; break;
			case STokens::ETokenIDs::else_:					out << "else"; break;
			//case STokens::ETokenIDs::while_:				out << "while"; break;
			//case STokens::ETokenIDs::return_:				out << "return"; break;

			case STokens::ETokenIDs::invalid:				out << "invalid"; break;

			case STokens::ETokenIDs::equal: out << "=="; break;
			case STokens::ETokenIDs::not_equal: out << "!="; break;
			case STokens::ETokenIDs::less: out << "<"; break;
			case STokens::ETokenIDs::less_equal: out << "<="; break;
			case STokens::ETokenIDs::greater: out << ">"; break;
			case STokens::ETokenIDs::greater_equal: out << ">="; break;

			case STokens::ETokenIDs::logical_or: out << "||"; break;
			case STokens::ETokenIDs::logical_and: out << "&&"; break;

			case STokens::ETokenIDs::bit_or: out << "|"; break;
			case STokens::ETokenIDs::bit_xor: out << "~"; break;
			case STokens::ETokenIDs::bit_and: out << "&"; break;
			case STokens::ETokenIDs::shift_left: out << "<<"; break;
			case STokens::ETokenIDs::shift_right: out << ">>"; break;
			case STokens::ETokenIDs::times: out << "*"; break;
			case STokens::ETokenIDs::divide: out << "/"; break;
			case STokens::ETokenIDs::mod: out << "%"; break;

			case STokens::ETokenIDs::plus: out << "+"; break;
			case STokens::ETokenIDs::minus: out << "-"; break;

			case STokens::ETokenIDs::not_: out << "!"; break;
			case STokens::ETokenIDs::stringify: out << "$"; break;
			//case STokens::ETokenIDs::compl_: out << "!"; break;

			case STokens::ETokenIDs::plus_plus: out << "++"; break;
			case STokens::ETokenIDs::minus_minus: out << "--"; break;

			case STokens::ETokenIDs::assign: out << "="; break;
			case STokens::ETokenIDs::bit_or_assign: out << "|="; break;
			case STokens::ETokenIDs::bit_xor_assign: out << "~="; break;
			case STokens::ETokenIDs::bit_and_assign: out << "&="; break;
			case STokens::ETokenIDs::shift_left_assign: out << "<<="; break;
			case STokens::ETokenIDs::shift_right_assign: out << ">>="; break;
			case STokens::ETokenIDs::times_assign: out << "*="; break;
			case STokens::ETokenIDs::divide_assign: out << "/="; break;
			case STokens::ETokenIDs::mod_assign: out << "%="; break;
			case STokens::ETokenIDs::plus_assign: out << "+="; break;
			case STokens::ETokenIDs::minus_assign: out << "-="; break;

			case STokens::ETokenIDs::lit_float: out << "lit_float"; break;
			case STokens::ETokenIDs::lit_uint: out << "lit_uint"; break;
			case STokens::ETokenIDs::lit_int: out << "lit_int"; break;
			case STokens::ETokenIDs::lit_boolean: out << "lit_boolean"; break;

			default: out << "unknown-token-ETokenIDs"; break;
		}
		return out;
	}
}
