#include "unit_test.h"

bool RoundingUnitTest::run_tests() {
	try {
		std::cout << "Running Rounding Test 1..." << std::endl;
		test1();
		std::cout << "OK " << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void RoundingUnitTest::test1() {
	//SSCFLSO instance = Generator::load_instance("instances/i300/i300_17.plc", false);
	SSCFLSO instance = Generator::load_instance("instances/i300/i300_17.plc", false);
	facility_vector res;
	Rounding RoundingAlgorithmObject = Rounding();
	RoundingAlgorithmObject.solve(instance, res, TWO_MINUTES, false);
}

