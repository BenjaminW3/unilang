#include <iostream>
#include <vector>
#include <string>

#include <filesystem>

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
namespace unilang
{
	void print_usage()
	{
		std::cout << "Available options:\n";
		std::cout << "  /compile filename.u out.asm  - Compile the specified program to the file out.asm\n";
		std::cout << std::endl;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main( int argc, char *argv[] ) 
{
   std::vector<std::string> vsParameters(&argv[1], &argv[argc]);	// The 0. argument is the command line itself so start with the first one

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
			if(vsParameters[i] == "/compile")
			{
				// read out the input file name
				if(++i >= vsParameters.size())
				{
					std::cout << "Error: Expected two filenames after /compile option. None was given.\n\n";
					break;
				}
				std::tr2::sys::path pInPath(vsParameters[i]);
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
				std::tr2::sys::path pOutPath(vsParameters[i]);

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
					// reserve the space in the string
					ifs.seekg(0, std::ios::end);   
					sSourceCode.reserve(static_cast<size_t>(ifs.tellg()));
					ifs.seekg(0, std::ios::beg);

					sSourceCode.assign((std::istreambuf_iterator<char>(ifs)),
										std::istreambuf_iterator<char>());
					ifs.close();

					// debug print file
					std::cout << sSourceCode << std::endl;
				
					// TODO: Compile source to bytecode
					void* bytecodebufferhandle = nullptr;

					if(bytecodebufferhandle)
					{
						//TODO: write out bytecode
					}
					else
					{
						throw std::runtime_error("Compilation failed!");
					}
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
		}
	}

   return 0;
}