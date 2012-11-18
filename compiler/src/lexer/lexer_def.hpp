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
		token_lexer<BaseIterator>::token_lexer() :	tok_whitespace("\\s+", STokens::whitespace),	// http://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
													tok_comment("(\\/\\*[^*]*\\*+([^/*][^*]*\\*+)*\\/)|(\\/\\/[^\r\n]*)", STokens::comment),
													tok_identifier("[a-zA-Z_][a-zA-Z_0-9]*", STokens::identifier),
													//tok_string("[^\"]+", STokens::string),
													lit_float("[-\\+]?(([1-9][0-9]*\\.[0-9]+)|(\\.[0-9]+))([eE][-\\+]?[0-9]+)?", STokens::lit_float),
													lit_uint("[1-9][0-9]*|0", STokens::lit_uint),
													lit_int("[-\\+][1-9][0-9]*|\\+0|-0", STokens::lit_int),
													lit_boolean("true|false", STokens::lit_boolean)
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

			add_("=",       STokens::assign);
			add_("\\+=",    STokens::plus_assign);
			add_("\\-=",    STokens::minus_assign);
			add_("\\*=",    STokens::times_assign);
			add_("\\/=",    STokens::divide_assign);
			add_("%=",      STokens::mod_assign);
			add_("\\&=",    STokens::bit_and_assign);
			add_("\\^=",    STokens::bit_xor_assign);
			add_("\\|=",    STokens::bit_or_assign);
			add_("<<=",     STokens::shift_left_assign);
			add_(">>=",     STokens::shift_right_assign);
			add_("\\|\\|",  STokens::logical_or);
			add_("&&",      STokens::logical_and);
			add_("\\|",     STokens::bit_or);
			add_("\\^",     STokens::bit_xor);
			add_("&",       STokens::bit_and);
			add_("<<",      STokens::shift_left);
			add_(">>",      STokens::shift_right);
			add_("==",      STokens::equal);
			add_("!=",      STokens::not_equal);
			add_("<",       STokens::less);
			add_("<=",      STokens::less_equal);
			add_(">",       STokens::greater);
			add_(">=",      STokens::greater_equal);
			add_("\\+",     STokens::plus);
			add_("\\-",     STokens::minus);
			add_("\\*",     STokens::times);
			add_("\\/",     STokens::divide);
			add_("%",       STokens::mod);
			add_("\\+\\+",  STokens::plus_plus);
			add_("\\-\\-",  STokens::minus_minus);
			//add_("~",       STokens::compl_);
			add_("!",       STokens::not_);
			add_("\\$",     STokens::stringify);
			add_(":",		STokens::colon);
			add_("->",		STokens::arrow);
			add_("if",		STokens::if_);
			add_("else",	STokens::else_);
			//add_("while",		STokens::while_);
			//add_("return",	STokens::return_);
			add_("\\(",		STokens::opening_parenthesis);
			add_("\\)",		STokens::closing_parenthesis);
			add_("\\{",		STokens::opening_brace);
			add_("\\}",		STokens::closing_brace);
			//add_("\\[",		STokens::opening_bracket);
			//add_("\\]",		STokens::closing_bracket);
			add_(",",		STokens::comma);
			add_(";",		STokens::semicolon);
			add_("~",		STokens::tilde);
			add_("\\?",		STokens::question_mark);
			
			this->self.add(lit_float);
			this->self.add(lit_uint);
			this->self.add(lit_int);
			this->self.add(lit_boolean);

			//this->self += lit_float | lit_uint | lit_int | lit_boolean;	// FIXME: does not work: + and - operators have to be matched before numbers

			this->self += tok_identifier;	// After adding symbols and keywords, so that identifiers do not use keywords
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		bool token_lexer<BaseIterator>::add_( std::string const& keyword, int id_ = STokens::invalid )
		{
			// add the token to the lexer
			STokens::ETokenIDs id;
			if (id_ == STokens::invalid)	{id = STokens::ETokenIDs(this->get_next_id());}
			else							{id = STokens::ETokenIDs(id_);}

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
			return raw_token((it != keywords_.end()) ? (*it).second : STokens::invalid);
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
			return token((it != keywords_.end()) ? (*it).second : STokens::invalid);
		}
	}
}