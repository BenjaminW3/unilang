#pragma warning(disable: 4913)		// user defined binary operator ',' exists but no overload could convert all operands, default built-in binary operator ',' used
#pragma warning(disable: 4503)		// 'boost::fusion::sequence_base<Sequence>' : decorated name length exceeded, name was truncated
#pragma warning(disable: 4714)		// function marked as __forceinline not inlined
#pragma warning(disable: 4510)		// default constructor could not be generated
#pragma warning(disable: 4610)		// struct can never be instantiated - user defined constructor required

#include <unilang/compiler/parser/grammar/ExpressionGrammarDef.hpp>

typedef std::string::const_iterator TSourceCodeIterator;
typedef unilang::lexer::CTokenLexer<TSourceCodeIterator> TLexer;
typedef TLexer::iterator_type TLexerTokenIterator;

template struct unilang::parser::SExpressionGrammar<TSourceCodeIterator, TLexerTokenIterator>;