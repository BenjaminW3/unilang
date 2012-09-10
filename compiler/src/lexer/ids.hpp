#pragma once

#include <ostream>

namespace unilang
{
    struct op_type
    {
        enum type
        {
            binary = 0x20000,
            unary = 0x40000,
            postfix_unary = 0x80000,
            assign = 0x100000
        };
    };

    struct op
    {
        enum type
        {
            // binary
            comma,
            assign,
            plus_assign,
            minus_assign,
            times_assign,
            divide_assign,
            mod_assign,
            bit_and_assign,
            bit_xor_assign,
            bit_or_assign,
            shift_left_assign,
            shift_right_assign,
            logical_or,
            logical_and,
            bit_or,
            bit_xor,
            bit_and,
            equal,
            not_equal,
            less,
            less_equal,
            greater,
            greater_equal,
            shift_left,
            shift_right,
            plus,
            minus,
            times,
            divide,
            mod,

            // unary
            plus_plus,
            minus_minus,
            //compl_,
            not_,
			stringify,
        };
    };

	inline std::ostream& operator<<(std::ostream& out, op::type const& x)
	{
		switch(x)
		{
			case op::type::comma: out << ","; break;
			case op::type::assign: out << "="; break;
			case op::type::plus_assign: out << "+="; break;
			case op::type::minus_assign: out << "-="; break;
			case op::type::times_assign: out << "*="; break;
			case op::type::divide_assign: out << "/="; break;
			case op::type::mod_assign: out << "%="; break;
			case op::type::bit_and_assign: out << "&="; break;
			case op::type::bit_xor_assign: out << "~="; break;
			case op::type::bit_or_assign: out << "|="; break;
			case op::type::shift_left_assign: out << "<<="; break;
			case op::type::shift_right_assign: out << ">>="; break;
			case op::type::logical_or: out << "||"; break;
			case op::type::logical_and: out << "&&"; break;
			case op::type::bit_or: out << "|"; break;
			case op::type::bit_xor: out << "~"; break;
			case op::type::bit_and: out << "&"; break;
			case op::type::equal: out << "=="; break;
			case op::type::not_equal: out << "!="; break;
			case op::type::less: out << "<"; break;
			case op::type::less_equal: out << "<="; break;
			case op::type::greater: out << ">"; break;
			case op::type::greater_equal: out << ">="; break;
			case op::type::shift_left: out << "<<"; break;
			case op::type::shift_right: out << ">>"; break;
			case op::type::plus: out << "+"; break;
			case op::type::minus: out << "-"; break;
			case op::type::times: out << "*"; break;
			case op::type::divide: out << "/"; break;
			case op::type::mod: out << "%"; break;
			case op::type::not_: out << "!"; break;
			//case op::type::compl_: out << "!"; break;
			case op::type::stringify: out << "$"; break;
			case op::type::plus_plus: out << "++"; break;
			case op::type::minus_minus: out << "--"; break;
			default: out << "unknown-operation"; break;
		}
		return out;
	}

    template <size_t type, size_t op>
    struct make_op
    {
        static size_t const value = type + op;
    };

    template <op::type op>
    struct unary_op : make_op<op_type::unary, op> {};

    template <op::type op>
    struct binary_op	: make_op<op_type::binary, op> {};

    template <op::type op>
	struct assign_op	: make_op<op_type::assign, op> {};

    template <op::type op>
    struct binary_or_unary_op	: make_op<op_type::unary | op_type::binary, op> {};

    struct token_ids
    {
        enum type
        {
            // pseudo tags
            invalid             = -1,
            op_binary           = op_type::binary,
            op_unary            = op_type::unary,
            op_assign           = op_type::assign,

            // binary / unary operators with common tokens
            // '+' and '-' can be binary or unary
            // (the lexer cannot distinguish which)
            plus                = binary_or_unary_op<op::type::plus>::value,
            minus               = binary_or_unary_op<op::type::minus>::value,

            // binary operators
            comma               = binary_op<op::type::comma>::value,
            assign              = assign_op<op::type::assign>::value,
            plus_assign         = assign_op<op::type::plus_assign>::value,
            minus_assign        = assign_op<op::type::minus_assign>::value,
            times_assign        = assign_op<op::type::times_assign>::value,
            divide_assign       = assign_op<op::type::divide_assign>::value,
            mod_assign          = assign_op<op::type::mod_assign>::value,
            bit_and_assign      = assign_op<op::type::bit_and_assign>::value,
            bit_xor_assign      = assign_op<op::type::bit_xor_assign>::value,
            bit_or_assign       = assign_op<op::type::bit_or_assign>::value,
            shift_left_assign   = assign_op<op::type::shift_left_assign>::value,
            shift_right_assign  = assign_op<op::type::shift_right_assign>::value,
            logical_or          = binary_op<op::type::logical_or>::value,
            logical_and         = binary_op<op::type::logical_and>::value,
            bit_or              = binary_op<op::type::bit_or>::value,
            bit_xor             = binary_op<op::type::bit_xor>::value,
            bit_and             = binary_op<op::type::bit_and>::value,
            equal               = binary_op<op::type::equal>::value,
            not_equal           = binary_op<op::type::not_equal>::value,
            less                = binary_op<op::type::less>::value,
            less_equal          = binary_op<op::type::less_equal>::value,
            greater             = binary_op<op::type::greater>::value,
            greater_equal       = binary_op<op::type::greater_equal>::value,
            shift_left          = binary_op<op::type::shift_left>::value,
            shift_right         = binary_op<op::type::shift_right>::value,
            times               = binary_op<op::type::times>::value,
            divide              = binary_op<op::type::divide>::value,
            mod                 = binary_op<op::type::mod>::value,

            // unary operators with overlaps
            // '++' and '--' can be prefix or postfix
            // (the lexer cannot distinguish which)
            plus_plus           = make_op<
                    op_type::unary
                |   op_type::postfix_unary, op::type::plus_plus>::value,

            minus_minus         = make_op<
                    op_type::unary
                |   op_type::postfix_unary, op::type::minus_minus>::value,

            // unary operators
			stringify			= unary_op<op::type::stringify>::value,
            //compl_              = unary_op<op::type::compl_>::value,
            not_                = unary_op<op::type::not_>::value,

            // misc tags
            identifier          = op::type::not_ + 1,
            comment,
            whitespace,
            colon,
            arrow,
            if_,
			else_,
            //while_,
			//return_,
            lit_float,
            lit_uint,
            lit_int,
            lit_boolean
        };
    };

	inline std::ostream& operator<<(std::ostream& out, token_ids::type const& x)
	{
		switch(x)
		{
			case token_ids::type::invalid: out << "invalid"; break;
			case token_ids::type::op_binary: out << "op_binary"; break;
			case token_ids::type::op_unary: out << "op_unary"; break;
			case token_ids::type::op_assign: out << "op_assign"; break;
			case token_ids::type::assign: out << "="; break;
			case token_ids::type::plus_assign: out << "+="; break;
			case token_ids::type::minus_assign: out << "-="; break;
			case token_ids::type::times_assign: out << "*="; break;
			case token_ids::type::divide_assign: out << "/="; break;
			case token_ids::type::mod_assign: out << "%="; break;
			case token_ids::type::bit_and_assign: out << "&="; break;
			case token_ids::type::bit_xor_assign: out << "~="; break;
			case token_ids::type::bit_or_assign: out << "|="; break;
			case token_ids::type::shift_left_assign: out << "<<="; break;
			case token_ids::type::shift_right_assign: out << ">>="; break;
			case token_ids::type::logical_or: out << "||"; break;
			case token_ids::type::logical_and: out << "&&"; break;
			case token_ids::type::bit_or: out << "|"; break;
			case token_ids::type::bit_xor: out << "~"; break;
			case token_ids::type::bit_and: out << "&"; break;
			case token_ids::type::equal: out << "=="; break;
			case token_ids::type::not_equal: out << "!="; break;
			case token_ids::type::less: out << "<"; break;
			case token_ids::type::less_equal: out << "<="; break;
			case token_ids::type::greater: out << ">"; break;
			case token_ids::type::greater_equal: out << ">="; break;
			case token_ids::type::shift_left: out << "<<"; break;
			case token_ids::type::shift_right: out << ">>"; break;
			case token_ids::type::plus: out << "+"; break;
			case token_ids::type::minus: out << "-"; break;
			case token_ids::type::times: out << "*"; break;
			case token_ids::type::divide: out << "/"; break;
			case token_ids::type::mod: out << "%"; break;
			case token_ids::type::not_: out << "!"; break;
			//case token_ids::type::compl_: out << "!"; break;
			case token_ids::type::stringify: out << "$"; break;
			case token_ids::type::plus_plus: out << "++"; break;
			case token_ids::type::minus_minus: out << "--"; break;
			case token_ids::type::identifier: out << "identifier"; break;
			case token_ids::type::comment: out << "comment"; break;
			case token_ids::type::whitespace: out << "whitespace"; break;
			case token_ids::type::colon: out << "colon"; break;
			case token_ids::type::arrow: out << "arrow"; break;
			case token_ids::type::if_: out << "if"; break;
			case token_ids::type::else_: out << "else"; break;
			//case token_ids::type::while_: out << "while"; break;
			//case token_ids::type::return_: out << "return"; break;
			case token_ids::type::lit_float: out << "lit_float"; break;
			case token_ids::type::lit_uint: out << "lit_uint"; break;
			case token_ids::type::lit_int: out << "lit_int"; break;
			case token_ids::type::lit_boolean: out << "lit_boolean"; break;
			default: out << "unknown-token-type"; break;
		}
		return out;
	}
}
