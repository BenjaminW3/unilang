#pragma once

#include <unilang/compiler/lexer/TokenIDsFwd.hpp>

#include <unilang/compiler/ast/OperatorsDef.hpp>

namespace unilang
{
	namespace lexer
	{
		namespace tokens
		{
			//#########################################################################
			//! Enumeration of all available token IDs.
			//! For all operations they match exactly the values from operators::EOperators.
			//#########################################################################
			enum class ETokenIDs : size_t
			{
				// pseudo tags
				invalid             = 0,

				// misc tags
				identifier          = 1,
				comment_singleline,
				comment_multiline_open,
				comment_multiline_recursive_open,
				comment_multiline_characters,
				comment_multiline_close,
				lit_string_delimiter,
				whitespace,
				comma,
				double_colon,
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
				namespace_,

				equal               = operators::EOperators::equal,	// from here on the the values should match op_type
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
				plus                = operators::EOperators::plus,
				minus,

				// unary operators
				not_                = operators::EOperators::not_,
				compl,
				stringify,

				// unary operators with overlaps '++' and '--' can be prefix or postfix (the lexer cannot distinguish which)
				plus_plus,
				minus_minus,

				// assignment operators
				assign              = operators::EOperators::assign,
				bit_or_assign       = operators::EOperators::bit_or_assign,
				bit_xor_assign,
				bit_and_assign,
				shift_left_assign,
				shift_right_assign,
				times_assign,
				divide_assign,
				mod_assign,
				plus_assign			= operators::EOperators::plus_assign,
				minus_assign,

				lit_string			= minus_assign + 1,
				lit_ufloat,
				lit_uint,
				lit_boolean,
			};
		}
	}
}