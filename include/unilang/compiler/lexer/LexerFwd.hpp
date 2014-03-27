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
# pragma warning(disable: 4267)	// 'return' : conversion from 'size_t' to 'unsigned int', possible loss of data
# pragma warning(disable: 4244)	// 'return' : conversion from 'const uint64_t' to 'unsigned int', possible loss of data
// TODO: Some of the disabled warnings form lexer.cpp could be moved to here?
#endif

#ifdef _DEBUG
#define BOOST_SPIRIT_LEXERTL_DEBUG 
#endif

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_PHOENIX_NO_PREDEFINED_TERMINALS

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/lex_lexertl_position_token.hpp>

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

#include <unilang/compiler/lexer/TokenIDsFwd.hpp>
#include <unilang/compiler/lexer/Literals.hpp>

#include <map>
#include <string>
#include <boost/algorithm/string/trim.hpp>

namespace unilang
{
	//-------------------------------------------------------------------------
	//! Rounds an integer to the next higher power of two. It keeps the value if it is already a perfect power of two.
	//-------------------------------------------------------------------------
	template<typename UnsignedInteger>
	UnsignedInteger roundUpToNextPowerOfTwo(UnsignedInteger value)
	{
		--value;
		for(size_t i = 1; i < sizeof(UnsignedInteger)* sizeof(char); i *= 2)
		{
			value |= value >> i;
		}
		return value+1;
	}
}

namespace boost
{ 
	namespace spirit 
	{ 
		namespace traits
		{
			//-------------------------------------------------------------------------
			//! Converts decimal string to unsigned integer.
			//-------------------------------------------------------------------------
			template <typename TSourceCodeIterator>
			struct assign_to_attribute_from_iterators<unilang::lexer::SIntLiteral, TSourceCodeIterator>
			{
				static void call(TSourceCodeIterator const & first, TSourceCodeIterator const & last, unilang::lexer::SIntLiteral & attr)
				{
					std::string sToken(first, last);
					// Remove all underscores.
					sToken.erase(std::remove_if(sToken.begin(), sToken.end(), [](char const & ch){return (ch == '_'); }), sToken.end());

					std::string const sPrefix(sToken.substr(0, 2));
					if(sPrefix=="0x")
					{
						attr.m_uiFullPrecisionValue.assign(sToken);
					}
					else if(sPrefix=="0q")
					{
						// Remove the q for boost::mp.
						std::string const sIntegerWithoutQPrefix("0" + sToken.substr(2));
						attr.m_uiFullPrecisionValue.assign(sIntegerWithoutQPrefix);
					}
					else if(sPrefix=="0b")
					{
						std::string const sIntegerWithoutPrefix(sToken.substr(2));

						// Initialize with zero.
						attr.m_uiFullPrecisionValue = 0;

						// Get a 2.
						decltype(attr.m_uiFullPrecisionValue) const uiMultiPrecTwo(2);

						// Iterate over the string back to front.
						size_t const uiStringSize(sIntegerWithoutPrefix.size());
						for(size_t uiIndex(0); uiIndex<uiStringSize; ++uiIndex)
						{
							if(sIntegerWithoutPrefix[uiStringSize -1 -uiIndex] == '1')
							{
								// If it is a one, add 2^index to the value.
								attr.m_uiFullPrecisionValue += boost::multiprecision::pow(uiMultiPrecTwo, uiIndex);
							}
						}
					}
					else
					{
						// We allow numbers starting with zeros, but boost::mp takes 0x... as hex and 0... as octal.
						boost::algorithm::trim_left_if(sToken, boost::is_any_of("0"));
						attr.m_uiFullPrecisionValue.assign(sToken);
					}

					auto const uiNumBits(64);	// FIXME: give int literal type explicitly
					auto const oldPrec (std::cout.precision());
					std::cout.precision(uiNumBits);
					std::cout << std::string(first, last) << " value: " << attr.m_uiFullPrecisionValue << " precision: " << uiNumBits << std::endl;
					std::cout.precision(oldPrec);

					attr.m_uiNumBitsType = unilang::roundUpToNextPowerOfTwo(uiNumBits);
					attr.m_bSignedType = true;	// FIXME: give int literal type explicitly
				}
			};

			//-------------------------------------------------------------------------
			//! Converts float string to APFloat.
			//-------------------------------------------------------------------------
			template <typename TSourceCodeIterator>
			struct assign_to_attribute_from_iterators<unilang::lexer::SFloatLiteral, TSourceCodeIterator>
			{
				static void call(TSourceCodeIterator const & first, TSourceCodeIterator const & last, unilang::lexer::SFloatLiteral & attr)
				{
					std::string sToken(first, last);
					// Remove all underscores.
					sToken.erase(std::remove_if(sToken.begin(), sToken.end(), [](char const & ch){return (ch == '_'); }), sToken.end());

					attr.m_uiFullPrecisionValue.assign(sToken);

					auto const uiNumBits(64);	// FIXME: give float literal type explicitly
					auto const oldPrec(std::cout.precision());
					std::cout.precision(uiNumBits);
					std::cout << std::string(first, last) << " value: " << attr.m_uiFullPrecisionValue << " precision: " << uiNumBits << std::endl;
					std::cout.precision(oldPrec);

					attr.m_uiNumBitsType = unilang::roundUpToNextPowerOfTwo(uiNumBits);
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
			//#########################################################################
			//! Static class holding the lexer and token types.
			//#########################################################################
			template <typename TSourceCodeIterator>
			struct get_lexer_type
			{
				// Our token needs to be able to carry several token values
				//TODO: remove all types other then std::string from the tokenizer! Value construction is to be done later!
				typedef boost::mpl::vector<std::string, SFloatLiteral, SIntLiteral, bool> token_value_types;	// Do NOT forget to update the context_type in LexerDef.hpp accordingly!

				// Using the position_token class as the token type to be returned from the lexer iterators allows to retain positional information as every token instance stores an iterator pair pointing to the matched input sequence.
				// The third parameter says if token definitions can use the state.
#ifdef TOKEN_ID
				typedef lex::lexertl::position_token< TSourceCodeIterator, token_value_types, boost::mpl::true_, tokens::ETokenIDs > token_type;
#else
				typedef lex::lexertl::position_token< TSourceCodeIterator, token_value_types, boost::mpl::true_ > token_type;
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
		template <typename TSourceCodeIterator>
		class CTokenLexer final :
			public lex::lexer<typename detail::get_lexer_type<TSourceCodeIterator>::type>,
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
			typedef TSourceCodeIterator base_iterator_type;
			
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			CTokenLexer();
			
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
			void addStringTokenToState(std::string const & keyword, tokens::ETokenIDs id, char const * state = nullptr, char const * targetstate = nullptr );

		public:
			typedef lex::token_def<boost::iterator_range<TSourceCodeIterator>, char, tokens::ETokenIDs> TToken;
#ifdef TOKEN_ID
			lex::token_def<std::string, char, tokens::ETokenIDs> const m_tokIdentifier;
			lex::token_def<std::string, char, tokens::ETokenIDs> const m_tokLiteralString;
			lex::token_def<SIntLiteral, char, tokens::ETokenIDs> const m_tokLiteralHexadecimal;
			lex::token_def<SIntLiteral, char, tokens::ETokenIDs> const m_tokLiteralOctal;
			lex::token_def<SIntLiteral, char, tokens::ETokenIDs> const m_tokLiteralBinary;
			lex::token_def<SFloatLiteral, char, tokens::ETokenIDs> const m_tokLiteralUnsignedFloat;
			lex::token_def<SIntLiteral, char, tokens::ETokenIDs> const m_tokLiteralUnsignedInt;
			lex::token_def<bool, char, tokens::ETokenIDs> const m_tokLiteralBoolean;
#else
			lex::token_def<std::string> const m_tokIdentifier;
			lex::token_def<std::string> const m_tokLiteralString;
			lex::token_def<SIntLiteral> const m_tokLiteralHexadecimal;
			lex::token_def<SIntLiteral> const m_tokLiteralOctal;
			lex::token_def<SIntLiteral> const m_tokLiteralBinary;
			lex::token_def<SFloatLiteral> const m_tokLiteralUnsignedFloat;
			lex::token_def<SIntLiteral> const m_tokLiteralUnsignedInt;
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