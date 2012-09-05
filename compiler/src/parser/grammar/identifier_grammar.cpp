#include "identifier_grammar_def.hpp"

typedef std::string::const_iterator base_iterator_type;
typedef unilang::lexer::token_lexer<base_iterator_type> lexer_type;
typedef lexer_type::iterator_type iterator_type;

template struct unilang::parser::identifier_grammar<base_iterator_type, iterator_type>;
