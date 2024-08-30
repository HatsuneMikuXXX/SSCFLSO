#include "composite_unit_test.h"

bool CompositeUnitTest::run_tests() {
	try {
		std::cout << "Running Composite Test 1. Expect a certain solution." << std::endl;
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


void CompositeUnitTest::test1() {
	Preprocess preprocess = Preprocess();
	LocalSearch local_search = LocalSearch(LocalSearch::GIVEN, LocalSearch::BEST);
	Composite composite(std::vector<Algorithm*>({&preprocess, &local_search}));
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc", true);
	expect_solution(&composite, instance, 2 * ONE_MINUTE, facility_vector({0, 1, 1, 1, 0}));
}