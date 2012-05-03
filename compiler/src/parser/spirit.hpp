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
// Uncomment this if you want to enable debugging
#define BOOST_SPIRIT_QI_DEBUG
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_function.hpp>