#pragma warning(disable: 4913)		// user defined binary operator ',' exists but no overload could convert all operands, default built-in binary operator ',' used
#pragma warning(disable: 4503)		// 'boost::fusion::sequence_base<Sequence>' : decorated name length exceeded, name was truncated

#include <unilang/compiler/parser/grammar/grammar_def.hpp>

typedef std::string::const_iterator base_iterator_type;
typedef unilang::lexer::token_lexer<base_iterator_type> lexer_type;
typedef lexer_type::iterator_type iterator_type;

template struct unilang::parser::global_grammar<base_iterator_type, iterator_type>;
