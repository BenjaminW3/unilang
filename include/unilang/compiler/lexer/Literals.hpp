#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <ostream>

namespace unilang
{
	namespace lexer
	{
		//#########################################################################
		//! A floating point type.
		//#########################################################################
		struct SFloatLiteral
		{
			boost::multiprecision::number<boost::multiprecision::cpp_dec_float<0>> m_uiFullPrecisionValue;
			size_t m_uiNumBitsType;
		};
		std::ostream& operator<<(std::ostream& out, SFloatLiteral const & x);

		//#########################################################################
		//! A integer
		//#########################################################################
		struct SIntLiteral
		{
			boost::multiprecision::number<boost::multiprecision::cpp_int_backend<0, 0, boost::multiprecision::signed_magnitude, boost::multiprecision::checked>> m_uiFullPrecisionValue;
			size_t m_uiNumBitsType;
			bool m_bSignedType;
		};
		std::ostream& operator<<(std::ostream& out, SIntLiteral const & x);
	}
}