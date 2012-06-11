#pragma once

///////////////////////////////////////////////////////////////////////////////
// Spirit v2.5 allows you to suppress automatic generation
// of predefined terminals to speed up complation. With
// BOOST_SPIRIT_NO_PREDEFINED_TERMINALS defined, you are
// responsible in creating instances of the terminals that
// you need (e.g. see qi::uint_type uint_ below).
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment this if you want to enable debuggin
#ifdef _DEBUG
	#define BOOST_SPIRIT_QI_DEBUG
#endif
///////////////////////////////////////////////////////////////////////////////

//#define BOOST_SPIRIT_USE_PHOENIX_V3 1

//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/spirit/include/phoenix_operator.hpp>
//#include <boost/spirit/include/phoenix_stl.hpp>
//#include <boost/spirit/include/phoenix_statement.hpp>
//#include <boost/spirit/home/phoenix/statement/if.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

#include <boost/spirit/include/qi.hpp>