#ifndef ALGORITHM_UNIT_TEST_H
#define ALGORITHM_UNIT_TEST_H
#include "algorithmClass.h"
#include "../SSCFLSO/SSCFLSO_generator.h"
#include <cassert>

namespace AlgorithmUnitTest{
	/// @brief Run all tests.
	/// @return Every test passed.
	bool run_tests();

	/// @brief Checks if preprocessing is implemented correctly.
	void test1();
}
#endif