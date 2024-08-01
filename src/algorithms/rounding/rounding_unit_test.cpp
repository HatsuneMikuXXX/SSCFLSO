#include "rounding_unit_test.h"

bool RoundingUnitTest::run_tests() {
	try {
		std::cout << "Running Rounding Test 1. Checking timelimit enforcement." << std::endl;
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

void RoundingUnitTest::test1() {
	Rounding rounding = Rounding();
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_i300.plc", true);
	check_timelimit_enforcement(&rounding, instance);
}