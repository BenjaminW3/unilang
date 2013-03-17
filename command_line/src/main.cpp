#include <iostream>
#include <vector>
#include <string>

#include "../../compiler/src/compiler.hpp"
#include "../../compiler/src/linker/linker.hpp"
#include "../../compiler/src/execution_engine/execution_engine.hpp"


#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// assignment operator could not be generated
#endif

#include <llvm/Support/CommandLine.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


//-----------------------------------------------------------------------------
//! \mainpage unilang-commandline
//! 
//! \section cmd_sec The command line tool
//!
//! \subsection cmd1 General
//! OVERVIEW: unilang compiler
//! 
//! Supports compilation, linkage and execution of unilang programs.
//! 
//! \subsection cmd2 Usage
//! USAGE: command_line.exe [options] <input files>
//! 
//! OPTIONS:
//   -def=<define=value> - A list of comma seperated preprocessor definitions either with value (COMPILER_VERSION=1) or without (DEBUG)
//!   -exec               - Execute the built module
//!   -help               - Display available options (-help-hidden for more)
//!   -o=<filename>       - Specify the output bitcode filename all given code is compiled into
//!   -print_options      - Prints the command line options given
//!   -version            - Display the version of this program
//!
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//! The main unilang namespace
//-----------------------------------------------------------------------------
namespace unilang
{
	//-----------------------------------------------------------------------------
	//! Prints out the information for command line usage
	//-----------------------------------------------------------------------------
	void print_version()
	{
		std::cout << "unilang compiler version 1.0";
		std::cout << std::endl;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
	// set own version printer not to print the llvm version number.
	llvm::cl::SetVersionPrinter(unilang::print_version);

#ifdef UL_ONE_INPUT_FILE
	llvm::cl::opt<std::string> sInputFilename(llvm::cl::Positional, llvm::cl::desc("<input file>"), llvm::cl::Required);
#else
	llvm::cl::list<std::string> vsInputFilenames(llvm::cl::Positional, llvm::cl::OneOrMore, llvm::cl::CommaSeparated, llvm::cl::Required, llvm::cl::desc("<input files>"));
#endif
	llvm::cl::opt<std::string> sOutputFilename("o", llvm::cl::desc("Specify the output bitcode filename all given code is compiled into"), llvm::cl::value_desc("filename"));
	//llvm::cl::list<std::string> sPreprocessorDefinitions("def", llvm::cl::ValueRequired, llvm::cl::Prefix, llvm::cl::ZeroOrMore, llvm::cl::CommaSeparated, llvm::cl::desc("A list of comma seperated preprocessor definitions either with value (COMPILER_VERSION=1) or without (DEBUG)"), llvm::cl::value_desc("define=value"));
	//llvm::cl::opt<bool> bQuiet		("quiet",			llvm::cl::ValueDisallowed,	llvm::cl::desc("Don't print informational messages"));
	//llvm::cl::list<std::string> sLibraryPaths("L", llvm::cl::Prefix, llvm::cl::CommaSeparated, llvm::cl::desc("Specify library search paths"), llvm::cl::value_desc("directories"));
	//llvm::cl::list<std::string> sLibraries("l", llvm::cl::Prefix, llvm::cl::desc("Specify libraries to link to"), llvm::cl::value_desc("libraries"));
	llvm::cl::opt<bool> bExecute		("exec",			llvm::cl::ValueDisallowed,	llvm::cl::desc("Execute the built module"));
	llvm::cl::opt<bool> bPrintOptions	("print_options",	llvm::cl::ValueDisallowed,	llvm::cl::desc("Prints the command line options given"));

	llvm::cl::ParseCommandLineOptions(argc, argv, "unilang compiler\n\nSupports compilation, linkage and execution of unilang programs.\n");

	// Print command line options
	if(bPrintOptions)
	{
		std::cout << "print_options: " << bPrintOptions << std::endl;
		std::cout << "exec: " << bExecute << std::endl;
		//std::cout << "quiet: " << bQuiet << std::endl;
		/*std::cout << "def: ";
		for(auto const & s : sPreprocessorDefinitions)
		{
			std::cout << s << " ";
		}
		std::cout << std::endl;*/
		std::cout << "o: " << sOutputFilename << std::endl;
#ifdef UL_ONE_INPUT_FILE
		std::cout << "input_filename: " << sInputFilename << std::endl;
#else
		std::cout << "input_filenames: ";
		for(auto const & s : vsInputFilenames)
		{
			std::cout << "'" << s << "' ";
		}
		std::cout << std::endl;
#endif
	}

	try
	{
		// compilation
#ifdef UL_ONE_INPUT_FILE
		std::shared_ptr<llvm::Module> spModule (unilang::compiler::compile_source_from_file(sInputFilename, unilang::compiler::EDebugOutputOptions::All));
#else
		std::vector<std::shared_ptr<llvm::Module>> vspModule (unilang::compiler::compile_source_from_files(vsInputFilenames, unilang::compiler::EDebugOutputOptions::All));
		std::shared_ptr<llvm::Module> spModule(unilang::linker::link_modules(vspModule));
#endif
		if(!spModule)
		{
			throw std::runtime_error("Invalid module returned from compile_source_from_file!");
		}
		else
		{
			// bitcode output
			if(!sOutputFilename.empty())
			{
				unilang::linker::write_module_to_bitcode_file(*spModule.get(), sOutputFilename);
			}

			// execution
			if(bExecute)
			{
				std::cout << "Execution returned: " << unilang::execution_engine::execute_module(*spModule.get()) << std::endl;
			}
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

	return 0;
}