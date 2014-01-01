#pragma warning(disable: 4913)		// user defined binary operator ',' exists but no overload could convert all operands, default built-in binary operator ',' used
#pragma warning(disable: 4503)		// 'boost::fusion::sequence_base<Sequence>' : decorated name length exceeded, name was truncated
#pragma warning(disable: 4714)		// function marked as __forceinline not inlined
#pragma warning(disable: 4510)		// default constructor could not be generated
#pragma warning(disable: 4610)		// struct can never be instantiated - user defined constructor required

#include <unilang/compiler/parser/grammar/function_grammar_def.hpp>

typedef std::string::const_iterator base_iterator_type;
typedef unilang::lexer::token_lexer<base_iterator_type> lexer_type;
typedef lexer_type::iterator_type iterator_type;

template struct unilang::parser::function_grammar<base_iterator_type, iterator_type>;
