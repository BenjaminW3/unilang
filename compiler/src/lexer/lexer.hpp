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
//#pragma warning(disable: 4717)		// function 'boost::spirit::lex::action<Subject,Action> boost::proto::transform<PrimitiveTransform>::operator ()<Expr&,const State&,Data&>(const boost::proto::exprns_::expr<Tag,Args,Arity>,const boost::fusion::nil,boost::spirit::unused_type) const' marked as __forceinline not inlined
#endif

#ifdef _DEBUG
#define BOOST_SPIRIT_LEXERTL_DEBUG 
#endif

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/lex_lexertl_position_token.hpp>

#include "token_ids.hpp"

#if defined LEXER_STATIC_TABLES
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "static_lexer.hpp"
#elif defined LEXER_STATIC_SWITCH
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "static_switch_lexer.hpp"
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace boost
{ 
	namespace spirit 
	{ 
		namespace traits
		{
			template <typename Iterator>
			struct assign_to_attribute_from_iterators<uint64_t, Iterator>
			{
				static void call(Iterator const& first, Iterator const& last, uint64_t& attr)
				{
					//boost::spirit::ulong_long_type uint_64_parser;
					//uint_parser<uint64_t, 10, 1, -1> uint_64_parser;
					//Iterator first_ = first;
					//qi::parse(first_, last, boost::spirit::qi::ulong_long(), attr2);
					
					std::string s(first, last);
					std::stringstream ss(s);
					ss >> attr;
				}
			};
		}
	}
}

namespace unilang 
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the lexer infrastructure.
	//-----------------------------------------------------------------------------
	namespace lexer
	{
		namespace lex = boost::spirit::lex;
		
		//-----------------------------------------------------------------------------
		//! The namespace defining the used lexer properties.
		//-----------------------------------------------------------------------------
		namespace detail
		{
			namespace lex = boost::spirit::lex;

			template <typename BaseIterator>
			struct get_lexer_type
			{
				// Our token needs to be able to carry several token values
				typedef boost::mpl::vector<std::string, long double, uint64_t, bool> token_value_types;

				// Using the position_token class as the token type to be returned from the lexer iterators allows to retain positional information
				// as every token instance stores an iterator pair pointing to the matched input sequence.
#ifdef TOKEN_ID
				typedef lex::lexertl::position_token< BaseIterator, token_value_types, boost::mpl::false_, tokens::ETokenIDs > token_type;
#else
				typedef lex::lexertl::position_token< BaseIterator, token_value_types, boost::mpl::false_ > token_type;
#endif

#if defined LEXER_DYNAMIC_TABLES
				// use the lexer based on runtime generated DFA tables
				typedef lex::lexertl::actor_lexer<token_type> type;
#elif defined LEXER_STATIC_TABLES
				// use the lexer based on pre-generated static DFA tables
				typedef lex::lexertl::static_actor_lexer<
					token_type
				  , boost::spirit::lex::lexertl::static_::lexer_conjure_static
				> type;
#elif defined LEXER_STATIC_SWITCH
				// use the lexer based on pre-generated static code
				typedef lex::lexertl::static_actor_lexer<
					token_type
				  , boost::spirit::lex::lexertl::static_::lexer_conjure_static_switch
				> type;
#else
	#error "Configuration problem: please select exactly one type of lexer to build"
#endif
			};
		}


		//#########################################################################
		//! The Lexer tokenizing the input stream for use by the parser.
		//#########################################################################
		template <typename BaseIterator>
		class token_lexer final :	public lex::lexer<typename detail::get_lexer_type<BaseIterator>::type>,
									boost::noncopyable
		{
		private:
#ifdef TOKEN_ID
			// get the type of any qi::raw_token(...) and qi::token(...) constructs
			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::raw_token(tokens::ETokenIDs) >::type raw_token_spec;

			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::token(tokens::ETokenIDs) >::type token_spec;

			typedef std::map<std::string, tokens::ETokenIDs> keyword_map_type;
#else
			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::raw_token(size_t) >::type raw_token_spec;

			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::token(size_t) >::type token_spec;

			typedef std::map<std::string, size_t> keyword_map_type;
#endif
		public:
			typedef BaseIterator base_iterator_type;
			
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			token_lexer();
			
			//-------------------------------------------------------------------------
			//! Extract a raw_token(id) for the given registered keyword
			//-------------------------------------------------------------------------
			raw_token_spec operator()(std::string const& kwd) const;
			
			//-------------------------------------------------------------------------
			//! Extract a token(id) for the given registered keyword
			//-------------------------------------------------------------------------
			token_spec token(std::string const& kwd) const;

		private:
			//-------------------------------------------------------------------------
			//! Adds a keyword to the mapping table
			//-------------------------------------------------------------------------
			bool add_( std::string const& keyword, tokens::ETokenIDs id );

		public:
#ifdef TOKEN_ID
			lex::token_def<lex::omit, char, tokens::ETokenIDs> const _tok_whitespace;
			lex::token_def<lex::omit, char, tokens::ETokenIDs> const _tok_comment;
			lex::token_def<std::string, char, tokens::ETokenIDs> const _tok_identifier;
			lex::token_def<long double, char, tokens::ETokenIDs> const _lit_ufloat;
			lex::token_def<uint64_t, char, tokens::ETokenIDs> const _lit_uint;
			lex::token_def<bool, char, tokens::ETokenIDs> const _lit_boolean;
#else
			lex::token_def<lex::omit> const _tok_whitespace;
			lex::token_def<lex::omit> const _tok_comment;
			lex::token_def<std::string> const _tok_identifier;
			lex::token_def<long double> const _lit_ufloat;
			lex::token_def<uint64_t> const _lit_uint;
			lex::token_def<bool> const _lit_boolean;
#endif

		private:
			keyword_map_type _keywords;
		};
	}
}