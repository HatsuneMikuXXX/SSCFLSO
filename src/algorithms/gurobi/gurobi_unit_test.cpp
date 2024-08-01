#include "gurobi_unit_test.h"

bool GurobiUnitTest::run_tests() {
	try {
		std::cout << "Running Gurobi Test 1. Checking timelimit enforcement. " << std::endl;
		print_current_system_time();
		test1();
		print_current_system_time();
		std::cout << "OK " << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "TEST FAILED!" << std::endl;
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void GurobiUnitTest::test1() {
	Gurobi gurobi = Gurobi();
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_i300.plc", true);
	check_timelimit_enforcement(&gurobi, instance);
}

