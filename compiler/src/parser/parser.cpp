#if defined(_MSC_VER)
#pragma warning(disable: 4127)	// warning C4127: conditional expression is constant
#pragma warning(disable: 4512)	// warning C4512: assignment operator could not be generated
#pragma warning(disable: 4100)	// warning C4100: 'x' : unreferenced formal parameter
#endif

#include "parser.hpp"

#include "grammar/function.hpp"
#include "skipper.hpp"

#include <iostream>
#include <chrono>

namespace unilang
{
	namespace parser
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		ast::function_list parse_code( std::string const & sSourceCode, error_handler<std::string::const_iterator> & error_handler )
		{
			// The AST we will return
			ast::function_list ast;

			// Take the time
			auto timePointBefore = std::chrono::system_clock::now();

			// initialise parsing
			std::string::const_iterator cIterBegin = sSourceCode.cbegin();
			std::string::const_iterator cIterEnd = sSourceCode.cend();
			function_grammar<std::string::const_iterator> parser(error_handler);
			skipper<std::string::const_iterator> skipper;
			
			// actually parse it now
			bool success = phrase_parse(cIterBegin, cIterEnd, +parser, skipper, ast);

			auto timePointAfter = std::chrono::system_clock::now();
			long long diff = std::chrono::duration_cast<std::chrono::duration<long, std::ratio<1, 1000>>>(timePointAfter - timePointBefore).count();
			std::cout << "Parsing duration: " << diff << " ms" << std::endl;

			// return the ast only if it was successfull
			if (success && cIterBegin == cIterEnd)
			{
				std::cout << "Parsing successful!\n";
				return ast;
			}
			else
			{
				throw std::runtime_error("Parsing failed!");
			}
		}
	}
}