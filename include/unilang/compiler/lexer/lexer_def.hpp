#pragma once

#include <unilang/compiler/lexer/lexer.hpp>

#include <unilang/compiler/lexer/token_ids_def.hpp>

#include <boost/spirit/include/phoenix_statement.hpp> // Otherwise only the last statement in the sequence of (comma separated) expressions will be executed.

namespace unilang
{ 
	namespace lexer
	{
		//-------------------------------------------------------------------------
		// Supported Regular Expressions http://www.boost.org/doc/libs/1_53_0/libs/spirit/doc/html/spirit/lex/quick_reference/lexer.html
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		token_lexer<BaseIterator>::token_lexer() :
			lex::lexer<typename detail::get_lexer_type<BaseIterator>::type>(lex::match_flags::match_default),	// match_default sets the matching of a '.' to match everything. Standard for lexertl is everything but no newline.
			m_uiCommentNestingLevel(0),
#ifdef TOKEN_ID
			m_tokWhitespace				("\\s+",															tokens::ETokenIDs::whitespace),	// http://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
			m_tokCommentSingleLine		("\\/\\/[^\r\n]*",													tokens::ETokenIDs::comment_singleline),
			m_tokCommentMultilineOpen	("\\/\\*",															tokens::ETokenIDs::comment_multiline_open),
			m_tokCommentMultilineRecursiveOpen("\\/\\*",													tokens::ETokenIDs::comment_multiline_recursive_open),
			m_tokCommentSingleLineInMultiline("\\/\\/[^\r\n]*",												tokens::ETokenIDs::comment_singleline),
			m_tokCommentMultilineCharacters("([^\\*\\/]|(\\/[^\\*\\/])|(\\*+[^\\*\\/]))+",					tokens::ETokenIDs::comment_multiline_characters),
			m_tokCommentMultilineClose	("\\*+\\/",															tokens::ETokenIDs::comment_multiline_close),
			m_tokIdentifier				("[a-zA-Z_][a-zA-Z_0-9]*",											tokens::ETokenIDs::identifier),
			//m_tokLiteralString					(R"(\"[^\"]*\")",											tokens::ETokenIDs::lit_string),
			m_tokLiteralStringDelimiter			("'[^']*'",													tokens::ETokenIDs::lit_string_delimiter),
			m_tokLiteralString					(".",														tokens::ETokenIDs::lit_string),
			m_tokLiteralHexadecimal		("0x[0-9a-fA-F]+",													tokens::ETokenIDs::lit_uint),
			m_tokLiteralOctal			("0q[0-8]+",														tokens::ETokenIDs::lit_uint),
			m_tokLiteralBinary			("0b[01]+",															tokens::ETokenIDs::lit_uint),
			m_tokLiteralUnsignedFloat	("((([1-9][0-9]*)|(0?))\\.[0-9]+)([eE][-\\+]?[0-9]+)?",				tokens::ETokenIDs::lit_ufloat),
			m_tokLiteralUnsignedInt		("[1-9][0-9]*|0",													tokens::ETokenIDs::lit_uint),
			m_tokLiteralBoolean			("true|false",														tokens::ETokenIDs::lit_boolean)
#else
			m_tokWhitespace						(R"(\s+)",													static_cast<size_t>(tokens::ETokenIDs::whitespace)),	// http://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
			m_tokCommentSingleLine				(R"(\/\/[^\r\n]*)",											static_cast<size_t>(tokens::ETokenIDs::comment_singleline)),
			m_tokCommentMultilineOpen			(R"(\/\*)",													static_cast<size_t>(tokens::ETokenIDs::comment_multiline_open)),
			m_tokCommentMultilineRecursiveOpen	(R"(\/\*)",													static_cast<size_t>(tokens::ETokenIDs::comment_multiline_recursive_open)),
			m_tokCommentSingleLineInMultiline	(R"(\/\/[^\r\n]*)",											static_cast<size_t>(tokens::ETokenIDs::comment_singleline)),
			m_tokCommentMultilineCharacters		(R"(([^\*\/]|(\/[^\*\/])|(\*+[^\*\/]))*)",					static_cast<size_t>(tokens::ETokenIDs::comment_multiline_characters)),
			m_tokCommentMultilineClose			(R"(\*+\/)",												static_cast<size_t>(tokens::ETokenIDs::comment_multiline_close)),
			m_tokIdentifier						("[a-zA-Z_][a-zA-Z_0-9]*",									static_cast<size_t>(tokens::ETokenIDs::identifier)),
			//m_tokLiteralString					(R"(\"[^\"]*\")",											static_cast<size_t>(tokens::ETokenIDs::lit_string)),
			m_tokLiteralStringDelimiter			("\'[^\']*\'",													static_cast<size_t>(tokens::ETokenIDs::lit_string_delimiter)),
			m_tokLiteralString					(".",														static_cast<size_t>(tokens::ETokenIDs::lit_string)),
			m_tokLiteralHexadecimal				("0x[0-9a-fA-F]+",											static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralOctal					("0q[0-8]+",												static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralBinary					("0b[01]+",													static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralUnsignedFloat			(R"(((([1-9][0-9]*)|(0?))\.[0-9]+)([eE][-\+]?[0-9]+)?)",	static_cast<size_t>(tokens::ETokenIDs::lit_ufloat)),
			m_tokLiteralUnsignedInt				("[1-9][0-9]*|0",											static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralBoolean					("true|false",												static_cast<size_t>(tokens::ETokenIDs::lit_boolean))
#endif
		{
			// The following tokens are associated with the default lexer state (the state "INITIAL" == initial_state().c_str() ). Specifying 'INITIAL' as a lexer state is strictly optional.

			typedef boost::spirit::lex::lexertl::detail::data<BaseIterator, boost::mpl::true_, boost::spirit::lex::lexertl::position_token<base_iterator_type, boost::spirit::lex::omit, boost::mpl::true_, size_t>::has_state, boost::variant<boost::detail::variant::over_sequence<boost::mpl::vector5<boost::iterator_range<std::_String_const_iterator<std::_String_val<std::_Simple_types<char>>>>, std::string, long double, unsigned __int64, bool>>>> context_type;

			// Define semantic action to ignore value.
			auto const fctPassIgnore(
				[](BaseIterator &, BaseIterator &, lex::pass_flags & pass, size_t &, context_type &)
				{
					pass = lex::pass_flags::pass_ignore;
				}
			);

			// Add single line comment before multiline so that you can comment them out like '//*' .
			this->self += m_tokCommentSingleLine[fctPassIgnore];

			// Define actions for multiline comments.
			auto const fctCommentMultilineOpen(
				[this](BaseIterator &, BaseIterator &, lex::pass_flags & pass, size_t &, context_type & ctx)
				{
					++m_uiCommentNestingLevel;
					ctx.set_state(ctx.get_state_id("MULTILINE_COMMENT"));
					pass = lex::pass_flags::pass_ignore;
				}
			);
			auto const fctCommentMultilineRecursiveOpen(
				[this](BaseIterator &, BaseIterator &, lex::pass_flags & pass, size_t &, context_type &)
				{
					++m_uiCommentNestingLevel;
					pass = lex::pass_flags::pass_ignore;
				}
			);
			auto const fctCommentMultilineClose(
				[this](BaseIterator &, BaseIterator &, lex::pass_flags & pass, size_t &, context_type & ctx)
				{
					--m_uiCommentNestingLevel;
					if(m_uiCommentNestingLevel == 0)
					{
						ctx.set_state(ctx.get_state_id("INITIAL"));
					}
					pass = lex::pass_flags::pass_ignore;
				}
			);
			// Add recursive multiline comment tokens.
			this->self += m_tokCommentMultilineOpen [fctCommentMultilineOpen];
			this->self("MULTILINE_COMMENT") = m_tokCommentMultilineRecursiveOpen	[fctCommentMultilineRecursiveOpen]	// Does not work with += ?!?
											| m_tokCommentMultilineClose			[fctCommentMultilineClose]
											| m_tokCommentSingleLineInMultiline		[fctPassIgnore]
											| m_tokCommentMultilineCharacters		[fctPassIgnore];

			// Define actions for raw strings.
			auto const fctLiteralRawStringDelimiter(
				[this](BaseIterator & itStart, BaseIterator & itEnd, lex::pass_flags & pass, size_t &, context_type & ctx)
				{
					m_sRawStringOpenDelimiterToken = std::string(itStart, itEnd);
					ctx.set_state(ctx.get_state_id("RAW_STRING"));
					// Ignore the opening string literal token.
					pass = lex::pass_flags::pass_ignore;
				}
			);
			auto const fctLiteralRawStringDelimiterEnd(
				[this](BaseIterator & itStart, BaseIterator & itEnd, lex::pass_flags & pass, size_t &, context_type & ctx)
				{
					m_sRawString += std::string(itStart, itEnd);
					size_t uiRawStringLength(m_sRawString.length());
					size_t uiDelimiterStringLength(m_sRawStringOpenDelimiterToken.length());
					size_t uiRawStringDelimiterBeginIndex(uiRawStringLength-uiDelimiterStringLength);
					if(uiRawStringLength>=uiDelimiterStringLength && m_sRawString.substr(uiRawStringDelimiterBeginIndex) == m_sRawStringOpenDelimiterToken)
					{
						ctx.set_value(m_sRawString.substr(0, uiRawStringDelimiterBeginIndex));
						ctx.set_state(ctx.get_state_id("INITIAL"));
						m_sRawStringOpenDelimiterToken.clear();
						m_sRawString.clear();
					}
					else
					{
						pass = lex::pass_flags::pass_ignore;
					}
				}
			);
			// Add raw string tokens.
			this->self += m_tokLiteralStringDelimiter[fctLiteralRawStringDelimiter];
			this->self("RAW_STRING")	= m_tokLiteralString[fctLiteralRawStringDelimiterEnd];

			this->self += m_tokWhitespace[fctPassIgnore];

			internal_add(":=",		tokens::ETokenIDs::assign);
			internal_add("\\*=",	tokens::ETokenIDs::times_assign);
			internal_add("\\/=",    tokens::ETokenIDs::divide_assign);
			internal_add("%=",      tokens::ETokenIDs::mod_assign);
			//internal_add("\\^=",    tokens::ETokenIDs::exponent_assign);
			internal_add("\\&=",    tokens::ETokenIDs::bit_and_assign);
			internal_add("\\#=",    tokens::ETokenIDs::bit_xor_assign);
			internal_add("\\|=",    tokens::ETokenIDs::bit_or_assign);
			internal_add("<<=",     tokens::ETokenIDs::shift_left_assign);
			internal_add(">>=",     tokens::ETokenIDs::shift_right_assign);
			internal_add("\\+=",    tokens::ETokenIDs::plus_assign);
			internal_add("\\-=",    tokens::ETokenIDs::minus_assign);
			internal_add("\\|\\|",  tokens::ETokenIDs::logical_or);
			internal_add("&&",      tokens::ETokenIDs::logical_and);
			internal_add("=",		tokens::ETokenIDs::equal);
			internal_add("!=",      tokens::ETokenIDs::not_equal);
			internal_add("<",       tokens::ETokenIDs::less);
			internal_add("<=",      tokens::ETokenIDs::less_equal);
			internal_add(">",       tokens::ETokenIDs::greater);
			internal_add(">=",      tokens::ETokenIDs::greater_equal);
			internal_add("\\*",     tokens::ETokenIDs::times);
			internal_add("\\/",     tokens::ETokenIDs::divide);
			internal_add("%",       tokens::ETokenIDs::mod);
			//internal_add("\\^",    tokens::ETokenIDs::exponent);
			internal_add("\\+",     tokens::ETokenIDs::plus);
			internal_add("\\-",     tokens::ETokenIDs::minus);
			internal_add("\\+\\+",  tokens::ETokenIDs::plus_plus);
			internal_add("\\-\\-",  tokens::ETokenIDs::minus_minus);
			internal_add("\\|",     tokens::ETokenIDs::bit_or);
			internal_add("\\#",     tokens::ETokenIDs::bit_xor);
			internal_add("&",       tokens::ETokenIDs::bit_and);
			internal_add("<<",      tokens::ETokenIDs::shift_left);
			internal_add(">>",      tokens::ETokenIDs::shift_right);
			internal_add("!",       tokens::ETokenIDs::not_);
			internal_add("~",       tokens::ETokenIDs::compl);
			internal_add("\\$",     tokens::ETokenIDs::stringify);
			internal_add("::",		tokens::ETokenIDs::double_colon);
			internal_add(":",		tokens::ETokenIDs::colon);
			internal_add("->",		tokens::ETokenIDs::arrow);
			internal_add("if",		tokens::ETokenIDs::if_);
			internal_add("else",	tokens::ETokenIDs::else_);
			//internal_add("while",		tokens::ETokenIDs::while_);
			//internal_add("return",	tokens::ETokenIDs::return_);
			internal_add("namespace",	tokens::ETokenIDs::namespace_);
			internal_add("\\(",		tokens::ETokenIDs::opening_parenthesis);
			internal_add("\\)",		tokens::ETokenIDs::closing_parenthesis);
			internal_add("\\{",		tokens::ETokenIDs::opening_brace);
			internal_add("\\}",		tokens::ETokenIDs::closing_brace);
			//internal_add("\\[",		tokens::ETokenIDs::opening_bracket);
			//internal_add("\\]",		tokens::ETokenIDs::closing_bracket);
			internal_add(",",		tokens::ETokenIDs::comma);
			internal_add(";",		tokens::ETokenIDs::semicolon);
			//internal_add("~",		tokens::ETokenIDs::tilde);
			internal_add("\\?",		tokens::ETokenIDs::question_mark);

			this->self += m_tokLiteralHexadecimal;
			this->self += m_tokLiteralOctal;
			this->self += m_tokLiteralBinary;
			this->self += m_tokLiteralUnsignedFloat;
			this->self += m_tokLiteralUnsignedInt;
			this->self += m_tokLiteralBoolean;

			this->self += m_tokIdentifier;	// After adding symbols and keywords, so that identifiers do not use keywords
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		void token_lexer<BaseIterator>::internal_add( std::string const & keyword, tokens::ETokenIDs id, char const * state, char const * targetstate )
		{
			assert(id != tokens::ETokenIDs::invalid);
			assert(!keyword.empty());
			
#ifdef TOKEN_ID
			add_token(state ? state : initial_state().c_str(), keyword, id, targetstate ? targetstate : state);
#else
			add_token(state ? state : initial_state().c_str(), keyword, static_cast<size_t>(id), targetstate ? targetstate : state);
#endif
		}
		//-------------------------------------------------------------------------
		// 
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		typename token_lexer<BaseIterator>::raw_token_spec token_lexer<BaseIterator>::operator()(tokens::ETokenIDs const & eTokenID) const
		//typename token_lexer<BaseIterator>::token_spec token_lexer<BaseIterator>::token(tokens::ETokenIDs const & eTokenID) const
		{
			namespace qi = boost::spirit::qi;
			qi::raw_token_type raw_token;
#ifdef TOKEN_ID
			return raw_token(eTokenID);
			//return token(eTokenID);
#else
			return raw_token(static_cast<size_t>(eTokenID));
			//return token(static_cast<size_t>(eTokenID));
#endif
		}
		//-------------------------------------------------------------------------
		//! \return The current comment nesting level.
		//-------------------------------------------------------------------------
		template <typename BaseIterator>
		size_t token_lexer<BaseIterator>::getCurrentCommentNestingLevel() const
		{
			return m_uiCommentNestingLevel;
		}
	}
}