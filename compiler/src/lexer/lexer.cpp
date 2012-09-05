#if defined(_MSC_VER)
//# pragma warning(disable: 4345)	// behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized
# pragma warning(disable: 4503)	// decorated name length exceeded, name was truncated
#endif

#include "lexer_def.hpp"

typedef std::string::const_iterator base_iterator_type;
template unilang::lexer::token_lexer<base_iterator_type>::token_lexer();
template bool unilang::lexer::token_lexer<base_iterator_type>::add_(std::string const&, int);
