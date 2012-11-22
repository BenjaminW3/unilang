#pragma once

#include <ostream>

namespace unilang
{
	namespace operators
	{
		//#########################################################################
		//! Enumeration of all available types of operations.
		//#########################################################################
		enum class EOperatorTypes : size_t;

		/*EOperatorTypes operator|(EOperatorTypes a, EOperatorTypes b);
		EOperatorTypes & operator|=(EOperatorTypes & a, EOperatorTypes b);
		EOperatorTypes operator&(EOperatorTypes a, EOperatorTypes b);
		EOperatorTypes & operator&=(EOperatorTypes& a, EOperatorTypes b);
		EOperatorTypes operator~(EOperatorTypes a);*/

		//#########################################################################
		//! Enumeration of all available operations.
		//#########################################################################
		enum class EOperators : size_t;

		/*EOperators operator|(EOperators a, EOperators b);
		EOperators & operator|=(EOperators & a, EOperators b);
		EOperators operator&(EOperators a, EOperators b);
		EOperators & operator&=(EOperators& a, EOperators b);
		EOperators operator~(EOperators a);*/

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
		size_t getPrecedenceOfOperator(EOperators op);

		std::ostream& operator<<(std::ostream& out, EOperators const& x);
	}
}
