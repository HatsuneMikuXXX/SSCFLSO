#include "greedy_unit_test.h"

bool GreedyUnitTest::run_tests() {
	try {
		std::cout << "Running Greedy Test 1. Expect a certain solution." << std::endl;
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

void GreedyUnitTest::test1() {
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc", true);
	Greedy greedy = Greedy();
	expect_solution(&greedy, instance, 2 * ONE_MINUTE, facility_vector({ 1, 1, 0, 0, 0 }));
}

