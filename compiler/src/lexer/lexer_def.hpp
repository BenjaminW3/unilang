#pragma once

#include "lexer.hpp"

#include "token_ids_def.hpp"

namespace unilang
{ 
	namespace lexer
	{
		//-------------------------------------------------------------------------
		// Supported Regular Expressions http://www.boost.org/doc/libs/1_46_0/libs/spirit/doc/html/spirit/lex/quick_reference/lexer.html
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
#ifdef TOKEN_ID
		token_lexer<BaseIterator>::token_lexer() :	_tok_whitespace("\\s+", tokens::ETokenIDs::whitespace),	// http://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
													_tok_comment("(\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/)|(\\/\\/[^\r\n]*)", tokens::ETokenIDs::comment),
													_tok_identifier("[a-zA-Z_][a-zA-Z_0-9]*", tokens::ETokenIDs::identifier),
													//tok_string("[^\"]+", tokens::ETokenIDs::string),
													_lit_ufloat("(([1-9][0-9]*\\.[0-9]+)|(\\.[0-9]+))([eE][-\\+]?[0-9]+)?", tokens::ETokenIDs::_lit_ufloat),
													//lit_float("[-\\+]?(([1-9][0-9]*\\.[0-9]+)|(\\.[0-9]+))([eE][-\\+]?[0-9]+)?", tokens::ETokenIDs::lit_float),
													_lit_uint("[1-9][0-9]*|0", tokens::ETokenIDs::_lit_uint),
													//lit_int("[-\\+][1-9][0-9]*|\\+0|-0", tokens::ETokenIDs::lit_int),
													_lit_boolean("true|false", tokens::ETokenIDs::_lit_boolean)
#else
		token_lexer<BaseIterator>::token_lexer() :	_tok_whitespace("\\s+", static_cast<size_t>(tokens::ETokenIDs::whitespace)),	// http://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
													_tok_comment("(\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/)|(\\/\\/[^\r\n]*)", static_cast<size_t>(tokens::ETokenIDs::comment)),
													_tok_identifier("[a-zA-Z_][a-zA-Z_0-9]*", static_cast<size_t>(tokens::ETokenIDs::identifier)),
													//tok_string("[^\"]+", tokens::ETokenIDs::string),
													_lit_ufloat("[-\\+]?((([1-9][0-9]*)|(0?))\\.[0-9]+)([eE][-\\+]?[0-9]+)?", static_cast<size_t>(tokens::ETokenIDs::lit_ufloat)),
													//lit_float("[-\\+]?(([1-9][0-9]*\\.[0-9]+)|(\\.[0-9]+))([eE][-\\+]?[0-9]+)?", tokens::ETokenIDs::lit_float),
													_lit_uint("[1-9][0-9]*|0", static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
													//lit_int("[-\\+][1-9][0-9]*|\\+0|-0", tokens::ETokenIDs::lit_int),
													_lit_boolean("true|false", static_cast<size_t>(tokens::ETokenIDs::lit_boolean))
#endif
													/*
													this->self.add_pattern
			("EXP",     "(e|E)(\\+|-)?\\d+")
			("FLOAT",    "-?(((\\d+)|(\\d*\\.\\d+)|(\\d+\\.\\d*))({EXP}|{SUFFIX})?)")
			("SYMBOL",  "[a-zA-Z_?@](\\w|\\?|@)*")
												*/
		{
			lex::_pass_type _pass;
			this->self += _tok_whitespace [lex::_pass = lex::pass_flags::pass_ignore];
			this->self += _tok_comment [lex::_pass = lex::pass_flags::pass_ignore];

			add_("=",       tokens::ETokenIDs::assign);
			add_("\\*=",    tokens::ETokenIDs::times_assign);
			add_("\\/=",    tokens::ETokenIDs::divide_assign);
			add_("%=",      tokens::ETokenIDs::mod_assign);
			//add_("\\^=",    tokens::ETokenIDs::exponent_assign);
			add_("\\&=",    tokens::ETokenIDs::bit_and_assign);
			add_("\\#=",    tokens::ETokenIDs::bit_xor_assign);
			add_("\\|=",    tokens::ETokenIDs::bit_or_assign);
			add_("<<=",     tokens::ETokenIDs::shift_left_assign);
			add_(">>=",     tokens::ETokenIDs::shift_right_assign);
			add_("\\+=",    tokens::ETokenIDs::plus_assign);
			add_("\\-=",    tokens::ETokenIDs::minus_assign);
			add_("\\|\\|",  tokens::ETokenIDs::logical_or);
			add_("&&",      tokens::ETokenIDs::logical_and);
			add_("==",      tokens::ETokenIDs::equal);
			add_("!=",      tokens::ETokenIDs::not_equal);
			add_("<",       tokens::ETokenIDs::less);
			add_("<=",      tokens::ETokenIDs::less_equal);
			add_(">",       tokens::ETokenIDs::greater);
			add_(">=",      tokens::ETokenIDs::greater_equal);
			add_("\\*",     tokens::ETokenIDs::times);
			add_("\\/",     tokens::ETokenIDs::divide);
			add_("%",       tokens::ETokenIDs::mod);
			//add_("\\^",    tokens::ETokenIDs::exponent);
			add_("\\+",     tokens::ETokenIDs::plus);
			add_("\\-",     tokens::ETokenIDs::minus);
			add_("\\+\\+",  tokens::ETokenIDs::plus_plus);
			add_("\\-\\-",  tokens::ETokenIDs::minus_minus);
			add_("\\|",     tokens::ETokenIDs::bit_or);
			add_("\\#",     tokens::ETokenIDs::bit_xor);
			add_("&",       tokens::ETokenIDs::bit_and);
			add_("<<",      tokens::ETokenIDs::shift_left);
			add_(">>",      tokens::ETokenIDs::shift_right);
			add_("!",       tokens::ETokenIDs::not_);
			add_("~",       tokens::ETokenIDs::compl);
			add_("\\$",     tokens::ETokenIDs::stringify);
			add_(":",		tokens::ETokenIDs::colon);
			add_("->",		tokens::ETokenIDs::arrow);
			add_("if",		tokens::ETokenIDs::if_);
			add_("else",	tokens::ETokenIDs::else_);
			//add_("while",		tokens::ETokenIDs::while_);
			//add_("return",	tokens::ETokenIDs::return_);
			add_("\\(",		tokens::ETokenIDs::opening_parenthesis);
			add_("\\)",		tokens::ETokenIDs::closing_parenthesis);
			add_("\\{",		tokens::ETokenIDs::opening_brace);
			add_("\\}",		tokens::ETokenIDs::closing_brace);
			//add_("\\[",		tokens::ETokenIDs::opening_bracket);
			//add_("\\]",		tokens::ETokenIDs::closing_bracket);
			add_(",",		tokens::ETokenIDs::comma);
			add_(";",		tokens::ETokenIDs::semicolon);
			add_("~",		tokens::ETokenIDs::tilde);
			add_("\\?",		tokens::ETokenIDs::question_mark);
			
			/*this->self.add(_lit_ufloat);
			this->self.add(_lit_uint);
			this->self.add(_lit_boolean);*/

			this->self += _lit_ufloat;
			this->self += _lit_uint;
			this->self += _lit_boolean;	// FIXME: returned value?

			this->self += _tok_identifier;	// After adding symbols and keywords, so that identifiers do not use keywords
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		bool token_lexer<BaseIterator>::add_( std::string const & keyword, tokens::ETokenIDs id )
		{
			assert(id != tokens::ETokenIDs::invalid);
			
#ifdef TOKEN_ID
			this->self.add(keyword, id);
			// store the mapping for later retrieval
			std::pair<typename keyword_map_type::iterator, bool> p = _keywords.insert(typename keyword_map_type::value_type(keyword, id));
#else
			this->self.add(keyword, static_cast<size_t>(id));
			// store the mapping for later retrieval
			std::pair<typename keyword_map_type::iterator, bool> p = _keywords.insert(typename keyword_map_type::value_type(keyword, static_cast<size_t>(id)));
#endif
			return p.second;
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		typename token_lexer<BaseIterator>::raw_token_spec token_lexer<BaseIterator>::operator()(std::string const& kwd) const
		{
			namespace qi = boost::spirit::qi;
			qi::raw_token_type raw_token;

			typename keyword_map_type::const_iterator it = _keywords.find(kwd);
			assert(it != _keywords.end());
#ifdef TOKEN_ID
			return raw_token((it != _keywords.end()) ? (*it).second : tokens::ETokenIDs::invalid);
#else
			return raw_token((it != _keywords.end()) ? (*it).second : static_cast<size_t>(tokens::ETokenIDs::invalid));
#endif
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		typename token_lexer<BaseIterator>::token_spec token_lexer<BaseIterator>::token(std::string const& kwd) const
		{
			namespace qi = boost::spirit::qi;
			qi::token_type token;

			typename keyword_map_type::const_iterator it = _keywords.find(kwd);
			assert(it != _keywords.end());
#ifdef TOKEN_ID
			return token((it != _keywords.end()) ? (*it).second : tokens::ETokenIDs::invalid);
#else
			return token((it != _keywords.end()) ? (*it).second : static_cast<size_t>(tokens::ETokenIDs::invalid));
#endif
		}
	}
}