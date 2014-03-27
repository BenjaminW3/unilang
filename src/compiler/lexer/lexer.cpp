#if defined(_MSC_VER)
#pragma warning(push)
# pragma warning(disable: 4503)	// decorated name length exceeded, name was truncated
# pragma warning(disable: 4510)	// warning C4510: 'boost::phoenix::actor<Expr>' : default constructor could not be generated
# pragma warning(disable: 4512)	// warning C4512: 'boost::phoenix::vector6<const boost::phoenix::actor<Expr> *,A0,A1,A2,A3,A4>' : assignment operator could not be generated
# pragma warning(disable: 4610)	// warning C4610 : struct 'boost::phoenix::actor<Expr>' can never be instantiated - user defined constructor required
#endif

#include <unilang/compiler/lexer/LexerDef.hpp>

typedef std::string::const_iterator TSourceCodeIterator;
template unilang::lexer::CTokenLexer<TSourceCodeIterator>::CTokenLexer();
template void unilang::lexer::CTokenLexer<TSourceCodeIterator>::addStringTokenToState(std::string const &, tokens::ETokenIDs, char const *, char const *);
template unilang::lexer::CTokenLexer<TSourceCodeIterator>::raw_token_spec unilang::lexer::CTokenLexer<TSourceCodeIterator>::operator()(tokens::ETokenIDs const & eTokenID) const;
template size_t unilang::lexer::CTokenLexer<TSourceCodeIterator>::getCurrentCommentNestingLevel() const;

/*
#include <fstream>
#include <iostream>

#include <unilang/compiler/lexer/LexerDef.hpp>
#include <boost/spirit/include/lex_generate_static_lexertl.hpp>

int main()
{
	typedef std::string::const_iterator TSourceCodeIterator;
	typedef client::lexer::conjure_tokens<TSourceCodeIterator> TLexer;

	TLexer lexer;

	// first generate the static switch based lexer 
	std::ofstream out_static("unilang_static_switch_lexer.hpp");

	bool result = boost::spirit::lex::lexertl::generate_static_switch(lexer, out_static, "unilang_static_switch");
	if (!result)
	{
		std::cerr << "Failed to generate static switch based lexer\n";
		return -1;
	}

	// now generate the static table based lexer 
	std::ofstream out("unilang_static_lexer.hpp");
	result = boost::spirit::lex::lexertl::generate_static( lexer, out, "unilang_static" );
	if (!result)
	{
		std::cerr << "Failed to generate static table based lexer\n";
		return -1;
	}

	return 0;
}
*/

#if defined(_MSC_VER)
#pragma warning(pop)
#endif