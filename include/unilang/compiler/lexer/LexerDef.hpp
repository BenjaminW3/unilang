#pragma once

#include <unilang/compiler/lexer/LexerFwd.hpp>

#include <unilang/compiler/lexer/TokenIDsDef.hpp>

#include <boost/spirit/include/phoenix_statement.hpp> // Otherwise only the last statement in the sequence of (comma separated) expressions will be executed.

namespace unilang
{ 
	namespace lexer
	{
		//-------------------------------------------------------------------------
		// Supported Regular Expressions http://www.boost.org/doc/libs/1_53_0/libs/spirit/doc/html/spirit/lex/quick_reference/lexer.html
		//-------------------------------------------------------------------------
		template <typename TSourceCodeIterator>
		CTokenLexer<TSourceCodeIterator>::CTokenLexer() :
			lex::lexer<typename detail::get_lexer_type<TSourceCodeIterator>::type>(lex::match_flags::match_default),	// match_default sets the matching of a '.' to match everything. Standard for lexertl is everything but no newline.
			m_uiCommentNestingLevel(0),
#ifdef TOKEN_ID
			m_tokWhitespace						(R"(\s+)",													tokens::ETokenIDs::whitespace),	// http://msdn.microsoft.com/en-us/library/6aw8xdf2.aspx
			m_tokCommentSingleLine				(R"(\/\/[^\r\n]*)",											tokens::ETokenIDs::comment_singleline),
			m_tokCommentMultilineOpen			(R"(\/\*)",													tokens::ETokenIDs::comment_multiline_open),
			m_tokCommentMultilineRecursiveOpen	(R"(\/\*)",													tokens::ETokenIDs::comment_multiline_recursive_open),
			m_tokCommentSingleLineInMultiline	(R"(\/\/[^\r\n]*)",											tokens::ETokenIDs::comment_singleline),
			m_tokCommentMultilineCharacters		(R"(([^\*\/]|(\/[^\*\/])|(\*+[^\*\/]))*)",					tokens::ETokenIDs::comment_multiline_characters),
			m_tokCommentMultilineClose			(R"(\*+\/)",												tokens::ETokenIDs::comment_multiline_close),
			m_tokIdentifier						("[a-zA-Z_][a-zA-Z_0-9]*",									tokens::ETokenIDs::identifier),
			//m_tokLiteralString					(R"(\"[^\"]*\")",											tokens::ETokenIDs::lit_string),
			m_tokLiteralStringDelimiter			("\'[^\']*\'",												tokens::ETokenIDs::lit_string_delimiter),
			m_tokLiteralString					(".",														tokens::ETokenIDs::lit_string),
			m_tokLiteralHexadecimal				("0x[_0-9a-fA-F]+",											tokens::ETokenIDs::lit_uint),
			m_tokLiteralOctal					("0q[_0-8]+",												tokens::ETokenIDs::lit_uint),
			m_tokLiteralBinary					("0b[_01]+",												tokens::ETokenIDs::lit_uint),
			m_tokLiteralUnsignedFloat			(R"(((([0-9][_0-9]*)?)\.[_0-9]+)([eE][-\+]?[_0-9]+)?)",		tokens::ETokenIDs::lit_ufloat),
			m_tokLiteralUnsignedInt				("[0-9][_0-9]*",											tokens::ETokenIDs::lit_uint),
			m_tokLiteralBoolean					("true|false",												tokens::ETokenIDs::lit_boolean)
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
			m_tokLiteralStringDelimiter			("\'[^\']*\'",												static_cast<size_t>(tokens::ETokenIDs::lit_string_delimiter)),
			m_tokLiteralString					(".",														static_cast<size_t>(tokens::ETokenIDs::lit_string)),
			m_tokLiteralHexadecimal				("0x[_0-9a-fA-F]+",											static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralOctal					("0q[_0-8]+",												static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralBinary					("0b[_01]+",												static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralUnsignedFloat			(R"(((([0-9][_0-9]*)?)\.[_0-9]+)([eE][-\+]?[_0-9]+)?)",		static_cast<size_t>(tokens::ETokenIDs::lit_ufloat)),
			m_tokLiteralUnsignedInt				("[0-9][_0-9]*",											static_cast<size_t>(tokens::ETokenIDs::lit_uint)),
			m_tokLiteralBoolean					("true|false",												static_cast<size_t>(tokens::ETokenIDs::lit_boolean))
#endif
		{
			// The following tokens are associated with the default lexer state (the state "INITIAL" == initial_state().c_str() ). Specifying 'INITIAL' as a lexer state is strictly optional.

				typedef boost::spirit::lex::lexertl::detail::data<TSourceCodeIterator, boost::mpl::true_, boost::spirit::lex::lexertl::position_token<TSourceCodeIterator, boost::spirit::lex::omit, boost::mpl::true_, size_t>::has_state, boost::variant<boost::detail::variant::over_sequence<boost::mpl::vector5<boost::iterator_range<std::_String_const_iterator<std::_String_val<std::_Simple_types<char>>>>, std::string, SFloatLiteral, SIntLiteral, bool>>>> context_type;

			// Define semantic action to ignore value.
			auto const fctPassIgnore(
				[](TSourceCodeIterator &, TSourceCodeIterator &, lex::pass_flags & pass, size_t &, context_type &)
				{
					pass = lex::pass_flags::pass_ignore;
				}
			);

			// Add single line comment before multiline so that you can comment them out like '//*' .
			this->self += m_tokCommentSingleLine[fctPassIgnore];

			// Define actions for multiline comments.
			auto const fctCommentMultilineOpen(
				[this](TSourceCodeIterator &, TSourceCodeIterator &, lex::pass_flags & pass, size_t &, context_type & ctx)
				{
					++m_uiCommentNestingLevel;
					ctx.set_state(ctx.get_state_id("MULTILINE_COMMENT"));
					pass = lex::pass_flags::pass_ignore;
				}
			);
			auto const fctCommentMultilineRecursiveOpen(
				[this](TSourceCodeIterator &, TSourceCodeIterator &, lex::pass_flags & pass, size_t &, context_type &)
				{
					++m_uiCommentNestingLevel;
					pass = lex::pass_flags::pass_ignore;
				}
			);
			auto const fctCommentMultilineClose(
				[this](TSourceCodeIterator &, TSourceCodeIterator &, lex::pass_flags & pass, size_t &, context_type & ctx)
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
				[this](TSourceCodeIterator & itStart, TSourceCodeIterator & itEnd, lex::pass_flags & pass, size_t &, context_type & ctx)
				{
					m_sRawStringOpenDelimiterToken = std::string(itStart, itEnd);
					ctx.set_state(ctx.get_state_id("RAW_STRING"));
					// Ignore the opening string literal token.
					pass = lex::pass_flags::pass_ignore;
				}
			);
			auto const fctLiteralRawStringDelimiterEnd(
				[this](TSourceCodeIterator & itStart, TSourceCodeIterator & itEnd, lex::pass_flags & pass, size_t &, context_type & ctx)
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

			addStringTokenToState(":=",		tokens::ETokenIDs::assign);
			addStringTokenToState("\\*=",	tokens::ETokenIDs::times_assign);
			addStringTokenToState("\\/=",    tokens::ETokenIDs::divide_assign);
			addStringTokenToState("%=",      tokens::ETokenIDs::mod_assign);
			addStringTokenToState("\\+=",    tokens::ETokenIDs::plus_assign);
			addStringTokenToState("\\-=",    tokens::ETokenIDs::minus_assign);
			//addStringTokenToState("\\^=",    tokens::ETokenIDs::exponent_assign);
			addStringTokenToState("\\&=",    tokens::ETokenIDs::bit_and_assign);
			addStringTokenToState("\\#=",    tokens::ETokenIDs::bit_xor_assign);
			addStringTokenToState("\\|=",    tokens::ETokenIDs::bit_or_assign);
			addStringTokenToState("<<=",     tokens::ETokenIDs::shift_left_assign);
			addStringTokenToState(">>=",     tokens::ETokenIDs::shift_right_assign);
			addStringTokenToState("\\|\\|",  tokens::ETokenIDs::logical_or);
			addStringTokenToState("&&",      tokens::ETokenIDs::logical_and);
			addStringTokenToState("=",		tokens::ETokenIDs::equal);
			addStringTokenToState("!=",      tokens::ETokenIDs::not_equal);
			addStringTokenToState("<",       tokens::ETokenIDs::less);
			addStringTokenToState("<=",      tokens::ETokenIDs::less_equal);
			addStringTokenToState(">",       tokens::ETokenIDs::greater);
			addStringTokenToState(">=",      tokens::ETokenIDs::greater_equal);
			addStringTokenToState("\\*",     tokens::ETokenIDs::times);
			addStringTokenToState("\\/",     tokens::ETokenIDs::divide);
			addStringTokenToState("%",       tokens::ETokenIDs::mod);
			//addStringTokenToState("\\^",    tokens::ETokenIDs::exponent);
			addStringTokenToState("\\+",     tokens::ETokenIDs::plus);
			addStringTokenToState("\\-",     tokens::ETokenIDs::minus);
			addStringTokenToState("\\+\\+",  tokens::ETokenIDs::plus_plus);
			addStringTokenToState("\\-\\-",  tokens::ETokenIDs::minus_minus);
			addStringTokenToState("&",       tokens::ETokenIDs::bit_and);
			addStringTokenToState("\\#",     tokens::ETokenIDs::bit_xor);
			addStringTokenToState("\\|",     tokens::ETokenIDs::bit_or);
			addStringTokenToState("<<",      tokens::ETokenIDs::shift_left);
			addStringTokenToState(">>",      tokens::ETokenIDs::shift_right);
			addStringTokenToState("!",       tokens::ETokenIDs::not_);
			addStringTokenToState("~",       tokens::ETokenIDs::compl);
			//addStringTokenToState("\\$",     tokens::ETokenIDs::stringify);
			addStringTokenToState("::",		tokens::ETokenIDs::double_colon);
			addStringTokenToState(":",		tokens::ETokenIDs::colon);
			addStringTokenToState("->",		tokens::ETokenIDs::arrow);
			addStringTokenToState("if",		tokens::ETokenIDs::if_);
			addStringTokenToState("else",	tokens::ETokenIDs::else_);
			//addStringTokenToState("while",		tokens::ETokenIDs::while_);
			//addStringTokenToState("return",	tokens::ETokenIDs::return_);
			addStringTokenToState("namespace",	tokens::ETokenIDs::namespace_);
			addStringTokenToState("\\(",		tokens::ETokenIDs::opening_parenthesis);
			addStringTokenToState("\\)",		tokens::ETokenIDs::closing_parenthesis);
			addStringTokenToState("\\{",		tokens::ETokenIDs::opening_brace);
			addStringTokenToState("\\}",		tokens::ETokenIDs::closing_brace);
			//addStringTokenToState("\\[",		tokens::ETokenIDs::opening_bracket);
			//addStringTokenToState("\\]",		tokens::ETokenIDs::closing_bracket);
			addStringTokenToState(",",		tokens::ETokenIDs::comma);
			addStringTokenToState(";",		tokens::ETokenIDs::semicolon);
			//addStringTokenToState("~",		tokens::ETokenIDs::tilde);
			addStringTokenToState("\\?",		tokens::ETokenIDs::question_mark);

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
		template <typename TSourceCodeIterator>
		void CTokenLexer<TSourceCodeIterator>::addStringTokenToState( std::string const & keyword, tokens::ETokenIDs id, char const * state, char const * targetstate )
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
		template <typename TSourceCodeIterator>
		typename CTokenLexer<TSourceCodeIterator>::raw_token_spec CTokenLexer<TSourceCodeIterator>::operator()(tokens::ETokenIDs const & eTokenID) const
		//typename CTokenLexer<TSourceCodeIterator>::token_spec CTokenLexer<TSourceCodeIterator>::token(tokens::ETokenIDs const & eTokenID) const
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
		template <typename TSourceCodeIterator>
		size_t CTokenLexer<TSourceCodeIterator>::getCurrentCommentNestingLevel() const
		{
			return m_uiCommentNestingLevel;
		}
	}
}