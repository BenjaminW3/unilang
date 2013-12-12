#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <tuple>

#include <unilang/compiler/compiler.hpp>
#include <unilang/compiler/execution_engine/execution_engine.hpp>

struct Test
{
	//-----------------------------------------------------------------------------
	//! Constructor.
	//-----------------------------------------------------------------------------
	Test(std::string sTestFile, bool bSucceeds, int64_t iReturnValue)
		:_sTestFile(sTestFile),
		_bSucceeds(bSucceeds),
		_iReturnValue(iReturnValue)
	{
	}

	std::string _sTestFile;
	bool _bSucceeds;
	int64_t _iReturnValue;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main( int /*argc*/, char * /*argv*/[] )
{
	std::string const sSourceDirectoryPath = "src/tests/code/";

	std::vector<Test> vTestFiles;
	vTestFiles.emplace_back("pass/pass_namespace_functions.u", true, 10);
	vTestFiles.emplace_back("pass/pass_assignment.u", true, 65);
	vTestFiles.emplace_back("pass/pass_singleline_comment.u", true, 36);
	vTestFiles.emplace_back("pass/pass_multiline_comment.u", true, 1+2+8+16);
	vTestFiles.emplace_back("pass/pass_boolean_type.u", true, 12);
	vTestFiles.emplace_back("pass/pass_int_literals.u", true, 1024+128);
	vTestFiles.emplace_back("pass/pass_int_rounding.u", true, 1+2+4+8);
	vTestFiles.emplace_back("pass/pass_int_initialization_list_const.u", true, 27);
	vTestFiles.emplace_back("pass/pass_int_initialization_list_nonconst.u", true, 23);
	vTestFiles.emplace_back("pass/pass_int_empty_initialization_list_const.u", true, 0);
	vTestFiles.emplace_back("pass/pass_int_empty_initialization_list_nonconst.u", true, 0);
	vTestFiles.emplace_back("pass/pass_operator_precedence.u", true, 1+2+4+8+16+32+64+128+256);
	vTestFiles.emplace_back("pass/pass_operator_decrement.u", true, 41);
	vTestFiles.emplace_back("pass/pass_operator_increment.u", true, 34);
	vTestFiles.emplace_back("pass/pass_operator_not.u", true, 3);
	vTestFiles.emplace_back("pass/pass_operator_compl.u", true, -3); // FIXME: operator compl same token as tilde
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_addition.u", true, 15);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_subtraction.u", true, 17);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_division.u", true, 2);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_multiplication.u", true, 8);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_shift_left.u", true, 4+8+16+32+64);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_shift_right.u", true, 1+2+4+8+16);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_bit_and.u", true, 8);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_bit_or.u", true, 8);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_bit_xor.u", true, 8);
	vTestFiles.emplace_back("pass/pass_operator_combined_assignment_mod.u", true, 3);
	vTestFiles.emplace_back("pass/pass_missing_return_variable_name.u", true, 77);
	
	vTestFiles.emplace_back("fail/fail_entrypoint_namespace.u", false, 0);
	vTestFiles.emplace_back("fail/fail_chained_assignment.u", false, 0);
	vTestFiles.emplace_back("fail/fail_assignment_expression.u", false, 0);
	vTestFiles.emplace_back("fail/fail_multiline_comment_unclosed.u", false, 0);
	vTestFiles.emplace_back("fail/fail_accessing_non_existant_function.u", false, 0);
	vTestFiles.emplace_back("fail/fail_accessing_non_existant_local_variable.u", false, 0);
	vTestFiles.emplace_back("fail/fail_duplicate_function_name.u", false, 0);
	vTestFiles.emplace_back("fail/fail_duplicate_parameter_name.u", false, 0);
	vTestFiles.emplace_back("fail/fail_duplicate_variable_name.u", false, 0);
	/*vTestFiles.emplace_back("fail/fail_higherorder_function_type_mismatch.u", false, 0);	//FIXME higherorder-type not recognized*/
	vTestFiles.emplace_back("fail/fail_int_initialization_list_too_long.u", false, 0);
	vTestFiles.emplace_back("fail/fail_literal_float_parameter_type_mismatch.u", false, 0);
	vTestFiles.emplace_back("fail/fail_literal_int_parameter_type_mismatch.u", false, 0);
	vTestFiles.emplace_back("fail/fail_mismatch_chained_assignment.u", false, 0);
	vTestFiles.emplace_back("fail/fail_missing_function_identifier.u", false, 0);
	vTestFiles.emplace_back("fail/fail_missing_variable_definition_type.u", false, 0);
	vTestFiles.emplace_back("fail/fail_use_of_variable_before_its_definition.u", false, 0);
	vTestFiles.emplace_back("fail/fail_variable_shadowing_variable_same_type.u", false, 0);
	vTestFiles.emplace_back("fail/fail_variable_shadowing_variable_different_type.u", false, 0);
	//vTestFiles.emplace_back("fail/fail_variable_shadowing_function.u", false, 0);	//FIXME: do not allow ?
	vTestFiles.emplace_back("fail/fail_variable_shadowing_keyword.u", false, 0);
	vTestFiles.emplace_back("fail/fail_write_to_non_mutable_variable.u", false, 0);
	
	std::vector<std::string> vFailedTests;

	for(auto const & test : vTestFiles)
	{
		try
		{
			std::shared_ptr<llvm::Module> spModule(unilang::compiler::compile_source_from_file(sSourceDirectoryPath + test._sTestFile));
			int64_t iRetVal (unilang::execution_engine::execute_module(*spModule.get()));

			if(test._bSucceeds)
			{
				if(test._iReturnValue == iRetVal)
				{
					std::cout << "Test: '" << test._sTestFile << "' passed." << std::endl << std::endl;
				}
				else
				{
					std::cout << "Test: '" << test._sTestFile << "' failed with wrong return value '" << iRetVal << "' instead of expected '" << test._iReturnValue << "'." << std::endl << std::endl;
					vFailedTests.push_back(test._sTestFile);
				}
			}
			else
			{
				std::cout << "Test: '" << test._sTestFile << "' succeeded but was not designed to succeed." << std::endl << std::endl;
				vFailedTests.push_back(test._sTestFile);
			}
		}
		catch(std::exception const & e)
		{
			std::cout << "Error: " << e.what() << std::endl;

			if(test._bSucceeds)
			{
				std::cout << "Test: '" << test._sTestFile << "' has thrown but was designed to succeed." << std::endl << std::endl;
				vFailedTests.push_back(test._sTestFile);
			}
			else
			{
				std::cout << "Test: '" << test._sTestFile << "' passed." << std::endl << std::endl;
			}
		}
		catch(...)
		{
			std::cout << "Unknown error!" << std::endl;

			std::cout << "Test: '" << test._sTestFile << "' has thrown unknown exception." << std::endl << std::endl;
			vFailedTests.push_back(test._sTestFile);
		}
	}
	
	std::cout << std::endl << std::endl  <<	"##########SUMMARY###########" << std::endl;
	for(std::string const & sFailedTests : vFailedTests)
	{
		std::cout<< "'" << sFailedTests << "' failed." << std::endl;
	}
	std::cout  << std::endl << "Failed tests: '" << vFailedTests.size() << std::endl;
	std::cout << std::endl  <<	"############################" << std::endl;
	assert(vFailedTests.size() == 0);

	return 0;
}