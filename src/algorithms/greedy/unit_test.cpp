#include "unit_test.h"

bool GreedyUnitTest::run_tests() {
	try {
		std::cout << "Running Greedy Test 1..." << std::endl;
		test1();
		std::cout << "OK " << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void GreedyUnitTest::test1() {
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
	Greedy g = Greedy();
	facility_vector expected_solution(5, 0);
	unit_test_expect_solution(&g, instance, 2 * ONE_MINUTE, expected_solution);
}

