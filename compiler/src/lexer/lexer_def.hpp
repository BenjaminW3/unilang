#pragma once

#include "lexer.hpp"

namespace unilang 
{ 
	namespace lexer
	{
		//-------------------------------------------------------------------------
		// Supported Regular Expressions http://www.boost.org/doc/libs/1_46_0/libs/spirit/doc/html/spirit/lex/quick_reference/lexer.html
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		token_lexer<BaseIterator>::token_lexer() :	tok_whitespace("\\s+", token_ids::whitespace),	// http://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
													tok_comment("(\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/)|(\\/\\/[^\r\n]*)", token_ids::comment),
													tok_identifier("[a-zA-Z_][a-zA-Z_0-9]*", token_ids::identifier),
													//tok_string("[^\"]+", token_ids::string),
													lit_float("[-\\+]?(([1-9][0-9]*\\.[0-9]+)|(\\.[0-9]+))([eE][-\\+]?[0-9]+)?", token_ids::lit_float),
													lit_uint("[1-9][0-9]*|0", token_ids::lit_uint),
													lit_int("[-\\+][1-9][0-9]*|\\+0|-0", token_ids::lit_int),
													lit_boolean("true|false", token_ids::lit_boolean)
													/*
													this->self.add_pattern
            ("EXP",     "(e|E)(\\+|-)?\\d+")
            ("FLOAT",    "-?(((\\d+)|(\\d*\\.\\d+)|(\\d+\\.\\d*))({EXP}|{SUFFIX})?)")
            ("SYMBOL",  "[a-zA-Z_?@](\\w|\\?|@)*")
												*/
		{
			lex::_pass_type _pass;
			this->self += tok_whitespace [lex::_pass = lex::pass_flags::pass_ignore];
			this->self += tok_comment [lex::_pass = lex::pass_flags::pass_ignore];

			this->self += lit_float | lit_uint | lit_int | lit_boolean;

			add_("=",       token_ids::assign);
			add_("\\+=",    token_ids::plus_assign);
			add_("\\-=",    token_ids::minus_assign);
			add_("\\*=",    token_ids::times_assign);
			add_("\\/=",    token_ids::divide_assign);
			add_("%=",      token_ids::mod_assign);
			add_("\\&=",    token_ids::bit_and_assign);
			add_("\\^=",    token_ids::bit_xor_assign);
			add_("\\|=",    token_ids::bit_or_assign);
			add_("<<=",     token_ids::shift_left_assign);
			add_(">>=",     token_ids::shift_right_assign);
			add_("\\|\\|",  token_ids::logical_or);
			add_("&&",      token_ids::logical_and);
			add_("\\|",     token_ids::bit_or);
			add_("\\^",     token_ids::bit_xor);
			add_("&",       token_ids::bit_and);
			add_("<<",      token_ids::shift_left);
			add_(">>",      token_ids::shift_right);
			add_("==",      token_ids::equal);
			add_("!=",      token_ids::not_equal);
			add_("<",       token_ids::less);
			add_("<=",      token_ids::less_equal);
			add_(">",       token_ids::greater);
			add_(">=",      token_ids::greater_equal);
			add_("\\+",     token_ids::plus);
			add_("\\-",     token_ids::minus);
			add_("\\*",     token_ids::times);
			add_("\\/",     token_ids::divide);
			add_("%",       token_ids::mod);
			add_("\\+\\+",  token_ids::plus_plus);
			add_("\\-\\-",  token_ids::minus_minus);
			//add_("~",       token_ids::compl_);
			add_("!",       token_ids::not_);
			add_("\\$",     token_ids::stringify);
			add_(":",		token_ids::colon);
			add_("->",		token_ids::arrow);
			add_("if",		token_ids::if_);
			add_("else",	token_ids::else_);
			//add_("while",		token_ids::while_);
			//add_("return",	token_ids::return_);
			add_("\\(",		token_ids::opening_parenthesis);
			add_("\\)",		token_ids::closing_parenthesis);
			add_("\\{",		token_ids::opening_brace);
			add_("\\}",		token_ids::closing_brace);
			//add_("\\[",		token_ids::opening_bracket);
			//add_("\\]",		token_ids::closing_bracket);
			add_(",",		token_ids::comma);
			add_(";",		token_ids::semicolon);
			add_("~",		token_ids::tilde);
			add_("\\?",		token_ids::question_mark);

			this->self += tok_identifier;	// After adding symbols and keywords, so that identifiers do not use keywords

			//this->self += lex::char_('(') | ')' | '{' | '}' | ',' | ';' | '~' | '?';
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		bool token_lexer<BaseIterator>::add_( std::string const& keyword, int id_ = token_ids::invalid )
		{
			// add the token to the lexer
			token_ids::type id;
			if (id_ == token_ids::invalid)	{id = token_ids::type(this->get_next_id());}
			else							{id = token_ids::type(id_);}

			this->self.add(keyword, id);
			// store the mapping for later retrieval
			std::pair<typename keyword_map_type::iterator, bool> p = keywords_.insert(typename keyword_map_type::value_type(keyword, id));

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

			typename keyword_map_type::const_iterator it = keywords_.find(kwd);
			assert(it != keywords_.end());
			return raw_token((it != keywords_.end()) ? (*it).second : token_ids::invalid);
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		typename token_lexer<BaseIterator>::token_spec token_lexer<BaseIterator>::token(std::string const& kwd) const
		{
			namespace qi = boost::spirit::qi;
			qi::token_type token;

			typename keyword_map_type::const_iterator it = keywords_.find(kwd);
			assert(it != keywords_.end());
			return token((it != keywords_.end()) ? (*it).second : token_ids::invalid);
		}
	}
}