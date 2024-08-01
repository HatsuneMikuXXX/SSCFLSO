#include "SLR_unit_test.h"

bool SemiLagrangianRelaxationUnitTest::run_tests() {
	try {
		std::cout << "Running SLR Test 1. Expect a certain solution." << std::endl;
		test1();
		std::cout << "OK" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "TEST FAILED!" << std::endl;
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void SemiLagrangianRelaxationUnitTest::test1() {
	SemiLagrangianRelaxation SLR = SemiLagrangianRelaxation(true);
	SSCFLSO instance = Generator::load_instance("instances/custom/example.plc", true);
	expect_solution(&SLR, instance, 10 * ONE_MINUTE, facility_vector({ 1, 1, 0, 0, 0 }));
}

