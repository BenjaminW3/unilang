#pragma once

///////////////////////////////////////////////////////////////////////////////
// The conjure lexer example can be built in 3 different variations:
//
//    - With a lexer using runtime generated DFA tables
//    - With a lexer using pre-generated (static) DFA tables
//    - With a lexer using a pre-generated custom switch based state machine
//
// Use one of the following preprocessor constants to define, which of those will be built:

///////////////////////////////////////////////////////////////////////////////
// Use the lexer based on runtime generated DFA tables
// #define LEXER_DYNAMIC_TABLES

///////////////////////////////////////////////////////////////////////////////
// Use the lexer based on pre-generated static DFA tables
// #define LEXER_STATIC_TABLES

///////////////////////////////////////////////////////////////////////////////
// Use the lexer based on runtime generated DFA tables
// #define LEXER_STATIC_SWITCH

///////////////////////////////////////////////////////////////////////////////
// The default is to use the dynamic table driven lexer
#if (!defined LEXER_DYNAMIC_TABLES) && (!defined LEXER_STATIC_TABLES) && (!defined LEXER_STATIC_SWITCH)
	#define LEXER_DYNAMIC_TABLES
#endif

///////////////////////////////////////////////////////////////////////////////
// Make sure we have only one lexer type selected
#if ((defined LEXER_DYNAMIC_TABLES) && (defined LEXER_STATIC_TABLES)) || \
	((defined LEXER_DYNAMIC_TABLES) && (defined LEXER_STATIC_SWITCH)) || \
	((defined LEXER_STATIC_TABLES) && (defined LEXER_STATIC_SWITCH))
#error "Configuration problem: please select exactly one type of lexer to build"
#endif

#if defined(_MSC_VER)
#pragma warning(push)
// TODO: Some of the disabled warnings form lexer.cpp could be moved to here?
#endif

#ifdef _DEBUG
#define BOOST_SPIRIT_LEXERTL_DEBUG 
#endif

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_PHOENIX_NO_PREDEFINED_TERMINALS

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/lex_lexertl_position_token.hpp>

#include <unilang/compiler/lexer/token_ids.hpp>

#if defined LEXER_STATIC_TABLES
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "static_lexer.hpp"
#elif defined LEXER_STATIC_SWITCH
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "static_switch_lexer.hpp"
#endif

#include <boost/spirit/include/qi_uint.hpp>			// qi::uint_parser

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <map>
#include <string>

namespace boost
{ 
	namespace spirit 
	{ 
		namespace traits
		{
			//-------------------------------------------------------------------------
			//! Converts decimal string to unsigned integer.
			//-------------------------------------------------------------------------
			template <typename BaseIterator>
			struct assign_to_attribute_from_iterators<uint64_t, BaseIterator>
			{
				static void call(BaseIterator const & first, BaseIterator const & last, uint64_t & attr)
				{
					std::string s(first, last);

					std::string sPrefix (s.substr(0,2));
					if(sPrefix=="0x")
					{
						qi::uint_parser<uint64_t, 16, 1, -1> hex_parser;
						boost::spirit::qi::parse(first, last, "0x" >> hex_parser, attr);
					}
					else if(sPrefix=="0q")
					{
						qi::uint_parser<uint64_t, 8, 1, -1> oct_parser;
						boost::spirit::qi::parse(first, last, "0q" >> oct_parser, attr);
					}
					else if(sPrefix=="0b")
					{
						qi::uint_parser<uint64_t, 2, 1, -1> bin_parser;
						boost::spirit::qi::parse(first, last, "0b" >> bin_parser, attr);
					}
					else
					{
						//uint64_t x;
						//uint_parser<uint64_t, 10, 1, -1> uint64_parser;
						//qi::parse(first_, last, uint64_parser, attr);
					
						//std::string s(first, last);
						std::stringstream ss(s);
						ss >> attr;
					}
				}
			};
		}
	}
}

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining the lexer infrastructure.
	//-------------------------------------------------------------------------
	namespace lexer
	{
		namespace lex = boost::spirit::lex;
		
		//-------------------------------------------------------------------------
		//! The namespace defining the used lexer properties.
		//-------------------------------------------------------------------------
		namespace detail
		{
			namespace lex = boost::spirit::lex;
			
			//#########################################################################
			//! Static class holding the lexer and token types.
			//#########################################################################
			template <typename BaseIterator>
			struct get_lexer_type
			{
				// Our token needs to be able to carry several token values
				typedef boost::mpl::vector<std::string, long double, uint64_t, bool> token_value_types;

				// Using the position_token class as the token type to be returned from the lexer iterators allows to retain positional information as every token instance stores an iterator pair pointing to the matched input sequence.
				// The third parameter says if token definitions can use the state.
#ifdef TOKEN_ID
				typedef lex::lexertl::position_token< BaseIterator, token_value_types, boost::mpl::true_, tokens::ETokenIDs > token_type;
#else
				typedef lex::lexertl::position_token< BaseIterator, token_value_types, boost::mpl::true_ > token_type;
#endif

#if defined LEXER_DYNAMIC_TABLES
				// use the lexer based on runtime generated DFA tables
				typedef lex::lexertl::actor_lexer<token_type> type;
#elif defined LEXER_STATIC_TABLES
				// use the lexer based on pre-generated static DFA tables
				typedef lex::lexertl::static_actor_lexer< token_type, boost::spirit::lex::lexertl::static_::lexer_conjure_static > type;
#elif defined LEXER_STATIC_SWITCH
				// use the lexer based on pre-generated static code
				typedef lex::lexertl::static_actor_lexer< token_type, boost::spirit::lex::lexertl::static_::lexer_conjure_static_switch > type;
#else
	#error "Configuration problem: please select exactly one type of lexer to build"
#endif
			};
		}


		//#########################################################################
		//! The Lexer tokenizing the input stream for use by the parser.
		//#########################################################################
		template <typename BaseIterator>
		class token_lexer final :
			public lex::lexer<typename detail::get_lexer_type<BaseIterator>::type>,
			boost::noncopyable
		{
		private:
#ifdef TOKEN_ID
			// get the type of any qi::raw_token(...) and qi::token(...) constructs
			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::raw_token(tokens::ETokenIDs) >::type raw_token_spec;
			//typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::token(tokens::ETokenIDs) >::type token_spec;
#else
			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::raw_token(size_t) >::type raw_token_spec;
			//typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::token(size_t) >::type token_spec;
#endif
		public:
			typedef BaseIterator base_iterator_type;
			
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			token_lexer();
			
			//-------------------------------------------------------------------------
			//! Extract a raw_token(id) for the given token-id.
			//-------------------------------------------------------------------------
			raw_token_spec operator()(tokens::ETokenIDs const & eTokenID) const;
			//token_spec token(std::string const & keyword) const;
			
			//-------------------------------------------------------------------------
			//! \return The current comment nesting level.
			//-------------------------------------------------------------------------
			size_t getCurrentCommentNestingLevel() const;

		private:
			//-------------------------------------------------------------------------
			//! Adds a keyword to the mapping table
			//-------------------------------------------------------------------------
			void internal_add(std::string const & keyword, tokens::ETokenIDs id, char const * state = nullptr, char const * targetstate = nullptr );

		public:
#ifdef TOKEN_ID
			lex::token_def<std::string, char, tokens::ETokenIDs> const m_tokIdentifier;
			lex::token_def<std::string, char, tokens::ETokenIDs> const m_tokLiteralString;
			lex::token_def<uint64_t, char, tokens::ETokenIDs> const m_tokLiteralHexadecimal;
			lex::token_def<uint64_t, char, tokens::ETokenIDs> const m_tokLiteralOctal;
			lex::token_def<uint64_t, char, tokens::ETokenIDs> const m_tokLiteralBinary;
			lex::token_def<long double, char, tokens::ETokenIDs> const m_tokLiteralUnsignedFloat;
			lex::token_def<uint64_t, char, tokens::ETokenIDs> const m_tokLiteralUnsignedInt;
			lex::token_def<bool, char, tokens::ETokenIDs> const m_tokLiteralBoolean;
#else
			lex::token_def<std::string> const m_tokIdentifier;
			lex::token_def<std::string> const m_tokLiteralString;
			lex::token_def<uint64_t> const m_tokLiteralHexadecimal;
			lex::token_def<uint64_t> const m_tokLiteralOctal;
			lex::token_def<uint64_t> const m_tokLiteralBinary;
			lex::token_def<long double> const m_tokLiteralUnsignedFloat;
			lex::token_def<uint64_t> const m_tokLiteralUnsignedInt;
			lex::token_def<bool> const m_tokLiteralBoolean;
#endif
		private:
#ifdef TOKEN_ID
			typedef lex::token_def<lex::omit, char, tokens::ETokenIDs> TConsumedToken;
			TConsumedToken const m_tokWhitespace;
			TConsumedToken const m_tokCommentSingleLine;
			TConsumedToken const m_tokCommentMultilineOpen;
			TConsumedToken const m_tokCommentMultilineRecursiveOpen;
			TConsumedToken const m_tokCommentSingleLineInMultiline;
			TConsumedToken const m_tokCommentMultilineCharacters;
			TConsumedToken const m_tokCommentMultilineClose;
			TConsumedToken const m_tokLiteralStringDelimiter;
#else
			typedef lex::token_def<lex::omit> TConsumedToken;
			TConsumedToken const m_tokWhitespace;
			TConsumedToken const m_tokCommentSingleLine;
			TConsumedToken const m_tokCommentMultilineOpen;
			TConsumedToken const m_tokCommentMultilineRecursiveOpen;
			TConsumedToken const m_tokCommentSingleLineInMultiline;
			TConsumedToken const m_tokCommentMultilineCharacters;
			TConsumedToken const m_tokCommentMultilineClose;
			TConsumedToken const m_tokLiteralStringDelimiter;
#endif
			size_t m_uiCommentNestingLevel;
			std::string m_sRawStringOpenDelimiterToken;
			std::string m_sRawString;
		};
	}
}