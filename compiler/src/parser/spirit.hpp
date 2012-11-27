#pragma once

///////////////////////////////////////////////////////////////////////////////
// Spirit v2.5 allows you to suppress automatic generation of predefined terminals to speed up complation. 
// With BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are responsible in creating instances of the terminals that you need.
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_PHOENIX_NO_PREDEFINED_TERMINALS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment this if you want to enable debugging
#ifdef _DEBUG
	#define BOOST_SPIRIT_QI_DEBUG
#endif
///////////////////////////////////////////////////////////////////////////////

// phoenix v3
#pragma warning(disable: 4714)		// function marked as __forceinline not inlined

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4512)		// assignment operator could not be generated

// phoenix v3
#pragma warning(disable: 4510)		// default constructor could not be generated
#pragma warning(disable: 4610)		// struct can never be instantiated - user defined constructor required
#endif

//#define BOOST_SPIRIT_UNICODE

#define BOOST_SPIRIT_USE_PHOENIX_V3 1
#define BOOST_RESULT_OF_USE_DECLTYPE

#include <boost/spirit/include/phoenix_function.hpp>

#include <boost/spirit/include/qi.hpp>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif