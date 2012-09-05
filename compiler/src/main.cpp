#include <iostream>
#include <vector>
#include <string>

#include <filesystem>

#include "parser/parser.hpp"
#include "code_generator/code_generator.hpp"

//-----------------------------------------------------------------------------
//! The main unilang namespace
//-----------------------------------------------------------------------------
namespace unilang
{
	//-----------------------------------------------------------------------------
	//! Prints out the information for command line usage
	//-----------------------------------------------------------------------------
	void print_usage()
	{
		std::cout << "Available options:\n";
		std::cout << "  /exec filename.u		       - Execute the specified program\n";
		//std::cout << "  /compile filename.u out.asm  - Compile the specified program to the file out.asm\n";
		std::cout << std::endl;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main( int argc, char *argv[] ) 
{
	// The 0. argument is the command line itself so start with the first one when creating the list
	std::vector<std::string> vsParameters(&argv[1], &argv[argc]);

	if(vsParameters.empty())
	{
		unilang::print_usage();
	}
	else
	{
		for(size_t i = 0; i < vsParameters.size(); ++i)
		{
#ifdef DEBUG
			std::cout << i << " " << vsParameters[i] << std::endl;
#endif
			if(vsParameters[i] == "/exec")
			{
				// read out the input file name
				if(++i >= vsParameters.size())
				{
					std::cout << "Error: Expected a filename after /exec option. None was given.\n\n";
					break;
				}
				std::tr2::sys::path const pInPath(vsParameters[i]);
				if(!std::tr2::sys::is_regular_file(pInPath))
				{
					std::cout << "Error: The input  for the /exec option is no valid file: \""+ pInPath.file_string() +"\"\n\n";
					break;
				}
				if(pInPath.extension() != ".u")
				{
					std::cout << "Error: The input  for the /exec option is no valid file. The '.u' extension is required: \""+ pInPath.file_string() +"\"\n\n";
					break;
				}

				try
				{
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

					// debug print file
					std::cout << std::endl << "#########SourceCode#########" << std::endl;
					std::cout << sSourceCode << std::endl;
					std::cout << "############################" << std::endl << std::endl;
					
					unilang::ast::module AST = unilang::parser::parse_code( sSourceCode );

					unilang::code_generator::code_generator gen( AST );
				}
				catch(const std::exception& e)
				{
					std::cout << "Error: " << e.what() << std::endl;
				}
				catch(...)
				{
					std::cout << "Unknown error!" << std::endl;
				}
			}
			/*else if(vsParameters[i] == "/compile")
			{
				// read out the input file name
				if(++i >= vsParameters.size())
				{
					std::cout << "Error: Expected two filenames after /compile option. None was given.\n\n";
					break;
				}
				std::tr2::sys::path const pInPath(vsParameters[i]);
				if(!std::tr2::sys::is_regular_file(pInPath))
				{
					std::cout << "Error: The input  for the /compile option is no valid file: \""+ pInPath.file_string() +"\"\n\n";
					break;
				}
				if(pInPath.extension() != ".u")
				{
					std::cout << "Error: The input  for the /compile option is no valid file. The '.u' extension is required: \""+ pInPath.file_string() +"\"\n\n";
					break;
				}

				// read out the ouput file name
				if(++i >= vsParameters.size())
				{
					std::cout << "Error: Expected two filenames after /compile option. One was given.\n\n";
					break;
				}
				std::tr2::sys::path const pOutPath(vsParameters[i]);

				try
				{
					std::cout << "Compiling: " << pInPath.file_string() << "\n";
					std::cout << "   Output: " << pOutPath.file_string() << "\n" << std::endl;
					 
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

					// debug print file
					std::cout << sSourceCode << std::endl;
					
					unilang::ast::module AST = unilang::parser::parse_code( sSourceCode );

					unilang::code_generator::code_generator gen( AST );
				}
				catch(const std::exception& e)
				{
					std::cout << "Error: " << e.what() << std::endl;
				}
				catch(...)
				{
					std::cout << "Unknown error!" << std::endl;
				}
			}*/
		}
	}

	return 0;
}