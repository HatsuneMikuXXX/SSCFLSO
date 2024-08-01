#include "preprocess_unit_test.h"

bool PreprocessUnitTest::run_tests() {
	try {
		std::cout << "Running Preprocess test 1. Checking correctness." << std::endl;
		test1();
		std::cout << "OK" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "TEST FAILED!" << std::endl;
		std::cerr << e.what()  << std::endl;
		return false;
	}
	return true;
}

void PreprocessUnitTest::test1() {
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_infeasible1.plc", true);
	Preprocess p = Preprocess();
	facility_vector expected_solution = facility_vector(100, 0);
	expect_solution(&p, instance, ONE_MINUTE, expected_solution);
	instance = Generator::load_instance("instances/unit_tests/unit_test_only69.plc", true);
	expected_solution[69] = 1;
	expect_solution(&p, instance, ONE_MINUTE, expected_solution);
	instance = Generator::load_instance("instances/unit_tests/unit_test_preprocess.plc", true);
	expected_solution.resize(2);
	expected_solution[1] = 1;
	expect_solution(&p, instance, ONE_MINUTE, expected_solution);
}