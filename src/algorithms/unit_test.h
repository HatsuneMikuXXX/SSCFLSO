#ifndef PREPROCESS_UNIT_TEST_H
#define PREPROCESS_UNIT_TEST_H
#include "preprocess.h"
#include "../SSCFLSO/SSCFLSO_generator.h"
namespace PreprocessUnitTest{
	/// @brief Run all tests.
	/// @return Every test passed.
	bool run_tests();

	/// @brief Checks if preprocessing is implemented correctly.
	void test1();
}
#endif