#if defined(_MSC_VER)
# pragma warning(disable: 4503)	// decorated name length exceeded, name was truncated
#endif

#include "lexer_def.hpp"

typedef std::string::const_iterator base_iterator_type;
template unilang::lexer::token_lexer<base_iterator_type>::token_lexer();
template bool unilang::lexer::token_lexer<base_iterator_type>::add_(std::string const&, tokens::ETokenIDs);
template unilang::lexer::token_lexer<base_iterator_type>::raw_token_spec unilang::lexer::token_lexer<base_iterator_type>::operator()(std::string const& kwd) const;
template unilang::lexer::token_lexer<base_iterator_type>::token_spec unilang::lexer::token_lexer<base_iterator_type>::token(std::string const& kwd) const;

/*
#include <fstream>
#include <iostream>

#include "lexer_def.hpp"
#include <boost/spirit/include/lex_generate_static_lexertl.hpp>

int main()
{
    typedef std::string::const_iterator base_iterator_type;
    typedef client::lexer::conjure_tokens<base_iterator_type> lexer_type;

    lexer_type lexer;

    // first generate the static switch based lexer 
    std::ofstream out_static("unilang_static_switch_lexer.hpp");

    bool result = boost::spirit::lex::lexertl::generate_static_switch(
        lexer, out_static, "unilang_static_switch");
    if (!result) {
        std::cerr << "Failed to generate static switch based lexer\n";
        return -1;
    }

    // now generate the static table based lexer 
    std::ofstream out("unilang_static_lexer.hpp");
    result = boost::spirit::lex::lexertl::generate_static(
        lexer, out, "unilang_static");
    if (!result) {
        std::cerr << "Failed to generate static table based lexer\n";
        return -1;
    }

    return 0;
}
*/