#include "simulated_annealing_unit_test.h"

bool SimulatedAnnealingUnitTest::run_tests() {
	try {
		std::cout << "Running Simulated Annealing Test 1. Checking timelimit enforcement." << std::endl;
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

void SimulatedAnnealingUnitTest::test1() {
	SimulatedAnnealing sim_ann = SimulatedAnnealing(SimulatedAnnealing::PREPROCESS);
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_i300.plc", true);
	check_timelimit_enforcement(&sim_ann, instance);
}
