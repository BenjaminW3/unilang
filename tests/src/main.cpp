#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <tuple>

#include "../../compiler/src/compiler.hpp"
#include "../../compiler/src/execution_engine/execution_engine.hpp"

struct Test
{
	std::string sTestFile;
	bool bSucceeds;
	int iReturnValue;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main( int /*argc*/, char * /*argv*/[] ) 
{
	std::vector<Test> vTestFiles;
	Test test_pass_chained_assignment = {"tests/code/pass_chained_assignment.u", true, 65};
	vTestFiles.push_back(test_pass_chained_assignment);
	Test test_pass_decrement = {"tests/code/pass_decrement.u", true, 41};
	vTestFiles.push_back(test_pass_decrement);
	Test test_pass_increment = {"tests/code/pass_increment.u", true, 34};
	vTestFiles.push_back(test_pass_increment);
	Test test_pass_int_initialization_list_const = {"tests/code/pass_int_initialization_list_const.u", true, 27};
	vTestFiles.push_back(test_pass_int_initialization_list_const);
	Test test_pass_int_initialization_list_nonconst = {"tests/code/pass_int_initialization_list_nonconst.u", true, 23};
	vTestFiles.push_back(test_pass_int_initialization_list_nonconst);
	Test test_pass_int_empty_initialization_list_const = {"tests/code/pass_int_empty_initialization_list_const.u", true, 0};
	vTestFiles.push_back(test_pass_int_empty_initialization_list_const);
	Test test_pass_int_empty_initialization_list_nonconst = {"tests/code/pass_int_empty_initialization_list_nonconst.u", true, 0};
	vTestFiles.push_back(test_pass_int_empty_initialization_list_nonconst);
	Test test_pass_missing_return_variable_name = {"tests/code/pass_missing_return_variable_name.u", true, 77};
	vTestFiles.push_back(test_pass_missing_return_variable_name);
	
	std::vector<std::string> vFailedTests;

	for(auto const & test : vTestFiles)
	{
		try
		{
			std::shared_ptr<llvm::Module> module = unilang::compiler::compile_file(test.sTestFile);
			int iRetVal = unilang::execution_engine::execute_module(module);

			if(test.bSucceeds)
			{
				if(test.iReturnValue == iRetVal)
				{
					std::cout << "Test: '" << test.sTestFile << "' passed." << std::endl;
				}
				else
				{
					std::cout << "Test: '" << test.sTestFile << "' failed with wrong return value '" << iRetVal << "' instead of expected '" << test.iReturnValue << "'." << std::endl;
					vFailedTests.push_back(test.sTestFile);
				}
			}
			else
			{
				std::cout << "Test: '" << test.sTestFile << "' succeeded but was not designed to succeed." << std::endl;
				vFailedTests.push_back(test.sTestFile);
			}
		}
		catch(const std::exception& e)
		{
			std::cout << "Error: " << e.what() << std::endl;

			if(test.bSucceeds)
			{
				std::cout << "Test: '" << test.sTestFile << "' has thrown but was designed to succeed." << std::endl;
				vFailedTests.push_back(test.sTestFile);
			}
		}
		catch(...)
		{
			std::cout << "Unknown error!" << std::endl;

			if(test.bSucceeds)
			{
				std::cout << "Test: '" << test.sTestFile << "' has thrown but was designed to succeed." << std::endl;
				vFailedTests.push_back(test.sTestFile);
			}
		}
	}
	
	std::cout << std::endl << std::endl  <<	"###########SUMMARY##########" << std::endl;
	for(std::string const & sFailedTests : vFailedTests)
	{
		std::cout<< "'" << sFailedTests << "' failed." << std::endl;
	}
	std::cout  << std::endl << "Failed tests: '" << vFailedTests.size() << std::endl;
	std::cout << std::endl  <<	"############################" << std::endl;
	assert(vFailedTests.size() == 0);

	return 0;
}