#pragma once

#include "operators.hpp"

namespace unilang
{
	namespace operators
	{
		//#########################################################################
		//! Enumeration of all available types of operations.
		//#########################################################################
		enum class EOperatorTypes : size_t
		{
			binaryOperation			= 0x20000,
			unaryOperation			= 0x40000,
			assignmentOperation		= 0x80000
		};

		//#########################################################################
		//! Enumeration of all available operations.
		//#########################################################################
		enum class EOperators : size_t
		{
			// binary
			equal               = static_cast<size_t>(EOperatorTypes::binaryOperation) + 1,
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

			plus				= static_cast<size_t>(EOperatorTypes::unaryOperation) | static_cast<size_t>(EOperatorTypes::binaryOperation) + 1,
			minus,

			// unary
			not_				= static_cast<size_t>(EOperatorTypes::unaryOperation) + 1,
			stringify,
			plus_plus,
			minus_minus,

			assign				= static_cast<size_t>(EOperatorTypes::assignmentOperation) + 1,
			bit_or_assign		= static_cast<size_t>(EOperatorTypes::assignmentOperation) + bit_or,
			bit_xor_assign,
			bit_and_assign,
			shift_left_assign,
			shift_right_assign,
			times_assign,
			divide_assign,
			mod_assign,
			plus_assign			= static_cast<size_t>(EOperatorTypes::assignmentOperation) | static_cast<size_t>(EOperatorTypes::unaryOperation) | static_cast<size_t>(EOperatorTypes::binaryOperation) + 1,
			minus_assign,
		};
	}
}
