#pragma once

#include "operators_def.hpp"

namespace unilang
{
	namespace operators
	{
		/*EOperatorTypes operator|(EOperatorTypes a, EOperatorTypes b){return static_cast<EOperatorTypes>(static_cast<size_t>(a) | static_cast<size_t>(b));}
		EOperatorTypes & operator|=(EOperatorTypes & a, EOperatorTypes b){a = a|b; return a;}
		EOperatorTypes operator&(EOperatorTypes a, EOperatorTypes b) {return static_cast<EOperatorTypes>(static_cast<size_t>(a) & static_cast<size_t>(b));}
		EOperatorTypes & operator&=(EOperatorTypes& a, EOperatorTypes b) {a = a & b; return a;}
		EOperatorTypes operator~(EOperatorTypes a) {return static_cast<EOperatorTypes>(~int(a));}

		EOperators operator|(EOperators a, EOperators b)			{return static_cast<EOperators>(static_cast<size_t>(a) | static_cast<size_t>(b));}
		EOperators & operator|=(EOperators & a, EOperators b)		{a = a|b; return a;}
		EOperators operator&(EOperators a, EOperators b)			{return static_cast<EOperators>(static_cast<size_t>(a) & static_cast<size_t>(b));}
		EOperators & operator&=(EOperators& a, EOperators b)		{a = a & b; return a;}
		EOperators operator~(EOperators a)							{return static_cast<EOperators>(~int(a));}*/

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		size_t getPrecedenceOfOperator(EOperators op)
		{
			switch(op)
			{
				case EOperators::equal:				return 8; break;
				case EOperators::not_equal:			return 8; break;
				case EOperators::less:				return 9; break;
				case EOperators::less_equal:		return 9; break;
				case EOperators::greater:			return 9; break;
				case EOperators::greater_equal:		return 9; break;

				case EOperators::logical_or:		return 3; break;
				case EOperators::logical_and:		return 4; break;

				case EOperators::bit_or:			return 5; break;
				case EOperators::bit_xor:			return 6; break;
				case EOperators::bit_and:			return 7; break;
				case EOperators::shift_left:		return 10; break;
				case EOperators::shift_right:		return 10; break;
				case EOperators::plus:				return 11; break;
				case EOperators::minus:				return 11; break;
				case EOperators::times:				return 12; break;
				case EOperators::divide:			return 12; break;
				case EOperators::mod:				return 12; break;

				// unary operators do not have precedence
				//case EOperators::not_: out << "!"; break;
				//case EOperators::stringify: out << "$"; break;
				//case EOperators::plus_plus: out << "++"; break;
				//case EOperators::minus_minus: out << "--"; break;

				case EOperators::assign:			return 2; break;
				case EOperators::bit_or_assign:		return 2; break;
				case EOperators::bit_xor_assign:	return 2; break;
				case EOperators::bit_and_assign:	return 2; break;
				case EOperators::shift_left_assign:	return 2; break;
				case EOperators::shift_right_assign:return 2; break;
				case EOperators::times_assign:		return 2; break;
				case EOperators::divide_assign:		return 2; break;
				case EOperators::mod_assign:		return 2; break;
				case EOperators::plus_assign:		return 2; break;
				case EOperators::minus_assign:		return 2; break;

				default: break;
			}
			return 0;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::ostream& operator<<(std::ostream& out, EOperators const& x)
		{
			switch(x)
			{
				case EOperators::equal:				out << "=="; break;
				case EOperators::not_equal:			out << "!="; break;
				case EOperators::less:				out << "<"; break;
				case EOperators::less_equal:		out << "<="; break;
				case EOperators::greater:			out << ">"; break;
				case EOperators::greater_equal:		out << ">="; break;

				case EOperators::logical_or:		out << "||"; break;
				case EOperators::logical_and:		out << "&&"; break;

				case EOperators::bit_or:			out << "|"; break;
				case EOperators::bit_xor:			out << "~"; break;
				case EOperators::bit_and:			out << "&"; break;
				case EOperators::shift_left:		out << "<<"; break;
				case EOperators::shift_right:		out << ">>"; break;
				case EOperators::plus:				out << "+"; break;
				case EOperators::minus:				out << "-"; break;
				case EOperators::times:				out << "*"; break;
				case EOperators::divide:			out << "/"; break;
				case EOperators::mod:				out << "%"; break;

				case EOperators::not_:				out << "!"; break;
				case EOperators::compl:				out << "~"; break;
				case EOperators::stringify:			out << "$"; break;
				case EOperators::plus_plus:			out << "++"; break;
				case EOperators::minus_minus:		out << "--"; break;

				case EOperators::assign:			out << "="; break;
				case EOperators::bit_or_assign:		out << "|="; break;
				case EOperators::bit_xor_assign:	out << "#="; break;
				case EOperators::bit_and_assign:	out << "&="; break;
				case EOperators::shift_left_assign:	out << "<<="; break;
				case EOperators::shift_right_assign:out << ">>="; break;
				case EOperators::times_assign:		out << "*="; break;
				case EOperators::divide_assign:		out << "/="; break;
				case EOperators::mod_assign:		out << "%="; break;
				case EOperators::plus_assign:		out << "+="; break;
				case EOperators::minus_assign:		out << "-="; break;

				default: out << "unknown-operation"; break;
			}
			return out;
		}
	}
}
