#include <iostream>
#include <vector>
#include <string>

#include "compiler.hpp"
#include "execution_engine/execution_engine.hpp"

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

				try
				{
					std::shared_ptr<llvm::Module> module = unilang::compiler::compile_file(vsParameters[i]);
					std::cout << "Execution returned: " << unilang::execution_engine::execute_module(module) << std::endl;
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
				
				try
				{
					std::shared_ptr<llvm::Module> module = unilang::compiler::compile_file(vsParameters[i]);

					// TODO: link to executable
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