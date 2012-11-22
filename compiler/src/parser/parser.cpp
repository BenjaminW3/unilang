#include <iostream>
#include <chrono>

#if defined(_MSC_VER)
#pragma warning(disable: 4127)		// warning C4127: conditional expression is constant
#pragma warning(disable: 4512)		// warning C4512: assignment operator could not be generated
#pragma warning(disable: 4100)		// warning C4100: 'x' : unreferenced formal parameter
#pragma warning(disable: 4913)		// warning C4913: user defined binary operator ',' exists but no overload could convert all operands, default built-in binary operator ',' used (..\..\src\parser\parser.cpp)
#pragma warning(disable: 4996)		// 'std::_Copy_impl': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
#endif

#include "parser.hpp"

#include "grammar/grammar.hpp"
#include "../lexer/lexer.hpp"
#include "error_handler.hpp"

#include "spirit.hpp"

namespace unilang
{
	namespace parser
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		ast::module parse_code( std::string const & sSourceCode )
		{
			std::cout << std::endl << "###########Parsing##########" << std::endl;

			// The AST we will return
			ast::module ast;

			// Take the time
			const auto timePointBefore = std::chrono::system_clock::now();

			// get the std::string iterators
			typedef std::string::const_iterator base_iterator_type;
			base_iterator_type cIterStringBegin = sSourceCode.cbegin();
			base_iterator_type cIterStringEnd = sSourceCode.cend();

			// calculate the lexer iterators used for parsing
			typedef unilang::lexer::token_lexer<base_iterator_type> lexer_type;
			lexer_type lexer;
			typedef lexer_type::iterator_type lexer_iterator_type;
			lexer_iterator_type cIterBegin = lexer.begin(cIterStringBegin, cIterStringEnd);
			lexer_iterator_type cIterEnd = lexer.end();

			// create error handler
			error_handler<base_iterator_type, lexer_iterator_type> error_handler(sSourceCode.cbegin(), sSourceCode.cend());

			// create parser
			global_grammar<base_iterator_type, lexer_iterator_type> parser(error_handler, lexer);
			
			// actually parse it now
			const bool success = boost::spirit::qi::parse(cIterBegin, cIterEnd, parser, ast);

			const auto timePointAfter = std::chrono::system_clock::now();
			const long long diff = std::chrono::duration_cast<std::chrono::duration<long, std::ratio<1, 1000>>>(timePointAfter - timePointBefore).count();
			std::cout << "Parsing duration: " << diff << " ms" << std::endl;

			// return the ast only if it was successfull
			if (success && cIterBegin == cIterEnd)
			{
				std::cout << "############################" << std::endl << std::endl;
				return ast;
			}
			else
			{
				throw std::runtime_error("Parsing failed!");
			}
		}
	}
}