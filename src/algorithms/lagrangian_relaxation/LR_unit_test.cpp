#include "LR_unit_test.h"

bool LagrangianRelaxationUnitTest::run_tests() {
	try {
		std::cout << "Running Lagrangian Relaxation Test 1. Checking timelimit enforcement." << std::endl;
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

void LagrangianRelaxationUnitTest::test1() {
	LagrangianRelaxation lr = LagrangianRelaxation();
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_medium.plc", true);
	check_timelimit_enforcement(&lr, instance);
}


