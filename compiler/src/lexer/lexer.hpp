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
// #define LEXER_DYNAMIC_TABLES 1

///////////////////////////////////////////////////////////////////////////////
// Use the lexer based on pre-generated static DFA tables
// #define LEXER_STATIC_TABLES 1

///////////////////////////////////////////////////////////////////////////////
// Use the lexer based on runtime generated DFA tables
// #define LEXER_STATIC_SWITCH 1

///////////////////////////////////////////////////////////////////////////////
// The default is to use the dynamic table driven lexer
#if LEXER_DYNAMIC_TABLES == 0 && \
	LEXER_STATIC_TABLES == 0 && \
	LEXER_STATIC_SWITCH == 0

#define LEXER_DYNAMIC_TABLES 1
#endif

///////////////////////////////////////////////////////////////////////////////
// Make sure we have only one lexer type selected
#if (LEXER_DYNAMIC_TABLES != 0 && LEXER_STATIC_TABLES != 0) || \
	(LEXER_DYNAMIC_TABLES != 0 && LEXER_STATIC_SWITCH != 0) || \
	(LEXER_STATIC_TABLES != 0 && LEXER_STATIC_SWITCH != 0)

#error "Configuration problem: please select exactly one type of lexer to build"
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4717)		// function 'boost::spirit::lex::action<Subject,Action> boost::proto::transform<PrimitiveTransform>::operator ()<Expr&,const State&,Data&>(const boost::proto::exprns_::expr<Tag,Args,Arity>,const boost::fusion::nil,boost::spirit::unused_type) const' marked as __forceinline not inlined
#endif

#ifdef _DEBUG
#define BOOST_SPIRIT_LEXERTL_DEBUG 
#endif

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/lex_lexertl_position_token.hpp>

#include "ids.hpp"

#if LEXER_STATIC_TABLES != 0
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "static_lexer.hpp"
#elif LEXER_STATIC_SWITCH != 0
#include <boost/spirit/include/lex_static_lexertl.hpp>
#include "static_switch_lexer.hpp"
#endif

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

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
				typedef boost::mpl::vector<std::string, long double, unsigned int, int, bool> token_value_types;

				// Using the position_token class as the token type to be returned from the lexer iterators allows to retain positional information
				// as every token instance stores an iterator pair pointing to the matched input sequence.
				typedef lex::lexertl::position_token< BaseIterator, token_value_types, boost::mpl::false_ > token_type;

#if LEXER_DYNAMIC_TABLES != 0
				// use the lexer based on runtime generated DFA tables
				typedef lex::lexertl::actor_lexer<token_type> type;
#elif LEXER_STATIC_TABLES != 0
				// use the lexer based on pre-generated static DFA tables
				typedef lex::lexertl::static_actor_lexer<
					token_type
				  , boost::spirit::lex::lexertl::static_::lexer_conjure_static
				> type;
#elif LEXER_STATIC_SWITCH != 0
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
		struct token_lexer	:	lex::lexer<typename detail::get_lexer_type<BaseIterator>::type>,
								boost::noncopyable
		{
		private:
			// get the type of any qi::raw_token(...) and qi::token(...) constructs
			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::raw_token(token_ids::type) >::type raw_token_spec;

			typedef typename boost::spirit::result_of::terminal< boost::spirit::tag::token(token_ids::type) >::type token_spec;

			typedef std::map<std::string, token_ids::type> keyword_map_type;

		protected:
			//-------------------------------------------------------------------------
			//! Adds a keyword to the mapping table
			//-------------------------------------------------------------------------
			bool add_( std::string const& keyword, int id_ = token_ids::invalid );

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

			lex::token_def<lex::omit> tok_whitespace;
			lex::token_def<lex::omit> tok_comment;
			lex::token_def<std::string> tok_identifier;
			lex::token_def<long double> lit_float;
			lex::token_def<unsigned int> lit_uint;
			lex::token_def<int> lit_int;
			lex::token_def<bool> lit_boolean;
			keyword_map_type keywords_;
		};
	}
}