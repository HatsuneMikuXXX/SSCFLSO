#include "tabu_unit_test.h"

bool TabuUnitTest::run_tests() {
	try {
		std::cout << "Running Tabu Test 1: Expect a certain solution." << std::endl;
		test1();
		std::cout << "OK " << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "TEST FAILED!" << std::endl;
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void TabuUnitTest::test1() {
	TabuSearch tabu = TabuSearch(TabuSearch::PREPROCESS);
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc", true);
	expect_solution(&tabu, instance, 2 * ONE_MINUTE, facility_vector({ 0, 1, 1, 1, 0 }));
}

