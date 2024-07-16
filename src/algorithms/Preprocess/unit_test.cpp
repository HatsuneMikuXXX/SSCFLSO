#include "unit_test.h"

bool PreprecessUnitTest::run_tests() {
	try {
		std::cout << "Running test 1. Checking correctness." << std::endl;
		test1();
		std::cout << "OK" << std::endl;
	}
	catch (...) {
		std::cerr << "Test failed." << std::endl;
		return false;
	}
	return true;
}

void PreprocessUnitTest::test1() {
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_infeasible1.plc");
	Preprocess p = Preprocess();
	facility_vector expected_solution = facility_vector(100, 0);
	expect_solution(&p, instance, ONE_MINUTE, expected_solution);
	instance = Generator::load_instance("instances/unit_tests/unit_test_only69.plc");
	expected_solution[69] = 1;
	expect_solution(&p, instance, ONE_MINUTE, expected_solution);
	instance = Generator::load_instance("instances/unit_tests/unit_test_preprocess.plc");
	expected_solution.resize(2);
	expected_solution[1] = 1;
	expect_solution(&p, instance, ONE_MINUTE, expected_solution);
}