#include "grasp_unit_test.h"

bool GRASPUnitTest::run_tests() {
	try {
		std::cout << "Running Grasp Test 1. Checking timelimit enforcement." << std::endl;
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

void GRASPUnitTest::test1() {
	GRASP grasp = GRASP(30, 0.5);
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_i300.plc", true);
	check_timelimit_enforcement(&grasp, instance);
}


