#ifndef HELPER_UNIT_TEST_H
#define HELPER_UNIT_TEST_H
#include "preprocess.h"
#include "../SSCFLSO/SSCFLSO_generator.h"
namespace HelpersUnitTest{
	/// @brief Run all tests.
	/// @return Every test passed.
	bool run_tests();

	/// @brief Checks if preprocessing is implemented correctly.
	void test1();
}
#endif