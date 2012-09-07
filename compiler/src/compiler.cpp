#include <iostream>
#include <vector>
#include <string>

#include <filesystem>

#include "parser/parser.hpp"
#include "code_generator/code_generator.hpp"

namespace unilang
{
	namespace compiler
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> compile_file( std::string const & sSourceCodeFilePath )
		{
			std::tr2::sys::path const pInPath(sSourceCodeFilePath);
			if(!std::tr2::sys::is_regular_file(pInPath))
			{
				throw std::runtime_error("The input is no valid file: \""+ pInPath.file_string() +"\"\n\n");
			}
			if(pInPath.extension() != ".u")
			{
				throw std::runtime_error("The input is no valid file. The '.u' extension is required: \""+ pInPath.file_string() +"\"\n\n");
			}

			std::cout << "Compiling: '" << pInPath.file_string() << "'\n";
					 
			std::ifstream ifs ( pInPath.file_string() , std::ifstream::in );
			if(!ifs)
			{
				throw std::runtime_error("Unable to open file: "+pInPath.file_string());
			}
			std::string sSourceCode;
			ifs.unsetf(std::ios::skipws); // No white space skipping!
			std::copy(
						std::istream_iterator<char>(ifs),
						std::istream_iterator<char>(),
						std::back_inserter(sSourceCode));
			ifs.close();

//#ifdef _DEBUG
			std::cout << std::endl << "#########SourceCode#########" << std::endl;
			std::cout << sSourceCode << std::endl;
			std::cout << "############################" << std::endl << std::endl;
//#endif	
			unilang::ast::module AST = unilang::parser::parse_code( sSourceCode );
			
			unilang::code_generator::code_generator gen( AST );
#ifdef _DEBUG
			std::cout << std::endl << "########unoptimized#########" << std::endl;
			gen.print_bytecode();
			std::cout << "############################" << std::endl << std::endl;
#endif
			gen.optimize();
//#ifdef _DEBUG
			std::cout << std::endl << "#########optimized##########" << std::endl;
			gen.print_bytecode();
			std::cout << "############################" << std::endl << std::endl;
//#endif
			return gen.GetModule();
		}
	}
}