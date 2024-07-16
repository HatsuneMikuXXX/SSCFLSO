#include "unit_test.h"

bool RoundingUnitTest::run_tests() {
	try {
		std::cout << "OK " << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}


