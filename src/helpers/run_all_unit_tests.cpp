#include "run_all_unit_tests.h"

namespace TESTER {
	bool run_all_tests() {
		std::cout << "################## Starting Unit Tests ##################" << std::endl;
		SSCFLSOUnitTest::run_tests();
		PreprocessUnitTest::run_tests();
		StingyUnitTest::run_tests();
		GreedyUnitTest::run_tests();
		std::cout << "All Tests Passed." << std::endl;
		return true;
	}
}