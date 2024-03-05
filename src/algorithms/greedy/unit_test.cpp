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
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_infeasible0.plc");
	facility_vector res;
	Greedy GreedyAlgorithmObject = Greedy();
	GreedyAlgorithmObject.solve(instance, res, TWO_MINUTES);
	if (sum(res) > 0) {
		throw std::runtime_error("Solution space is empty but Greedy returned a feasible solution");
	}
	instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
	GreedyAlgorithmObject.solve(instance, res, TWO_MINUTES);
	if (!(sum(res) == 2 && res[0] == 1 && res[1] == 1)) {
		throw std::runtime_error("The returned solution should be of size 2 and contain at least the facilities 1 and 2.");
	}
}

