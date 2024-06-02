#ifndef SSCFLSO_UNIT_TEST_H
#define SSCFLSO_UNIT_TEST_H
#include "SSCFLSO_validator.h"
#include "SSCFLSO_generator.h"
#include "../helpers/utility.h"
/// @brief Test the functionalities of the modules `Validator` and `Generator`.
namespace SSCFLSOUnitTest{
	/// @brief Run all tests.
	/// @return Every test passed.
	bool run_tests();
	
	/// @brief Checks creation and manipulation of an instance.
	void test1();

	/// @brief Checks saving and loading of instances (*.plc files).
	void test2();

	/// @brief Checks validation, assignments, computed costs, and concrete violations (i.e. which facilities). 
	void test3();

	/// @brief Checks if dropping facilities works correctly
	void test4();
}
#endif