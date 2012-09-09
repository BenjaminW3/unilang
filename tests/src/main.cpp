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

	Test test_fail_accessing_non_existant_function = {"tests/code/fail_accessing_non_existant_function.u", false, 0};
	vTestFiles.push_back(test_fail_accessing_non_existant_function);
	Test test_fail_accessing_non_existant_local_variable = {"tests/code/fail_accessing_non_existant_local_variable.u", false, 0};
	vTestFiles.push_back(test_fail_accessing_non_existant_local_variable);
	Test test_fail_compound_statement_variable_shadowing = {"tests/code/fail_compound_statement_variable_shadowing.u", false, 0};
	vTestFiles.push_back(test_fail_compound_statement_variable_shadowing);
	Test test_fail_duplicate_function_name = {"tests/code/fail_duplicate_function_name.u", false, 0};
	vTestFiles.push_back(test_fail_duplicate_function_name);
	Test test_fail_duplicate_parameter_name = {"tests/code/fail_duplicate_parameter_name.u", false, 0};
	vTestFiles.push_back(test_fail_duplicate_parameter_name);
	Test test_fail_duplicate_variable_name = {"tests/code/fail_duplicate_variable_name.u", false, 0};
	vTestFiles.push_back(test_fail_duplicate_variable_name);
	Test test_fail_higherorder_function_type_mismatch = {"tests/code/fail_higherorder_function_type_mismatch.u", false, 0};
	vTestFiles.push_back(test_fail_higherorder_function_type_mismatch);
	Test test_fail_int_initialization_list_too_long = {"tests/code/fail_int_initialization_list_too_long.u", false, 0};
	vTestFiles.push_back(test_fail_int_initialization_list_too_long);
	Test test_fail_literal_float_parameter_type_mismatch = {"tests/code/fail_literal_float_parameter_type_mismatch.u", false, 0};
	vTestFiles.push_back(test_fail_literal_float_parameter_type_mismatch);
	Test test_fail_literal_int_parameter_type_mismatch = {"tests/code/fail_literal_int_parameter_type_mismatch.u", false, 0};
	vTestFiles.push_back(test_fail_literal_int_parameter_type_mismatch);
	Test test_fail_literal_uint_parameter_type_mismatch = {"tests/code/fail_literal_uint_parameter_type_mismatch.u", false, 0};
	vTestFiles.push_back(test_fail_literal_uint_parameter_type_mismatch);
	Test test_fail_mismatch_chained_assignment = {"tests/code/fail_mismatch_chained_assignment.u", false, 0};
	vTestFiles.push_back(test_fail_mismatch_chained_assignment);
	Test test_fail_missing_function_identifier = {"tests/code/fail_missing_function_identifier.u", false, 0};
	vTestFiles.push_back(test_fail_missing_function_identifier);
	Test test_fail_missing_variable_definition_type = {"tests/code/fail_missing_variable_definition_type.u", false, 0};
	vTestFiles.push_back(test_fail_missing_variable_definition_type);
	Test test_fail_use_of_variable_before_its_definition = {"tests/code/fail_use_of_variable_before_its_definition.u", false, 0};
	vTestFiles.push_back(test_fail_use_of_variable_before_its_definition);
	Test test_fail_variable_shadowing_function = {"tests/code/fail_variable_shadowing_function.u", false, 0};
	vTestFiles.push_back(test_fail_variable_shadowing_function);
	Test test_fail_variable_shadowing_keyword = {"tests/code/fail_variable_shadowing_keyword.u", false, 0};
	vTestFiles.push_back(test_fail_variable_shadowing_keyword);
	Test test_fail_write_to_non_mutable_variable = {"tests/code/fail_write_to_non_mutable_variable.u", false, 0};
	vTestFiles.push_back(test_fail_write_to_non_mutable_variable);
	
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

			std::cout << "Test: '" << test.sTestFile << "' has thrown unknown exception." << std::endl;
			vFailedTests.push_back(test.sTestFile);
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