#include "compiler.hpp"

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
		std::string read_source_from_file( std::string const & sSourceCodeFilePath )
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

			return sSourceCode;
		}

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> compile_source( std::string const & sSourceCode, EDebugOutputOptions const output )
		{
			/*if((EDebugOutputOptions::SourceCode & output) == EDebugOutputOptions::SourceCode)
			{
				std::cout << std::endl << "#########SourceCode#########" << std::endl;
				std::cout << sSourceCode << std::endl;
				std::cout << "############################" << std::endl << std::endl;
			}*/

			unilang::ast::module AST (unilang::parser::parse_code( sSourceCode ));
			
			unilang::code_generator::code_generator gen( AST );

			if((EDebugOutputOptions::Unoptimized & output) == EDebugOutputOptions::Unoptimized)
			{
				std::cout << std::endl << "########unoptimized#########" << std::endl;
				gen.getllvmCodeGenerator().printModuleBytecode();
				std::cout << "############################" << std::endl << std::endl;
			}

			gen.getllvmCodeGenerator().optimizeModule();

			if((EDebugOutputOptions::Optimized & output) == EDebugOutputOptions::Optimized)
			{
				std::cout << std::endl << "#########optimized##########" << std::endl;
				gen.getllvmCodeGenerator().printModuleBytecode();
				std::cout << "############################" << std::endl << std::endl;
			}

			return gen.getllvmCodeGenerator().getModule();
		}

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> compile_source_from_file( std::string const & sSourceCodeFilePath, EDebugOutputOptions const output )
		{
			return compile_source(read_source_from_file(sSourceCodeFilePath), output);
		}
		
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::vector<std::shared_ptr<llvm::Module>> compile_source_from_files( std::vector<std::string> const & vsSourceCodeFilePaths, EDebugOutputOptions const output )
		{
			std::vector<std::shared_ptr<llvm::Module>> vspModules;
			
			for(auto const & sSourceFilePath : vsSourceCodeFilePaths)
			{
				vspModules.push_back(compile_source_from_file(sSourceFilePath, output));
			}

			return vspModules;
		}
	}
}