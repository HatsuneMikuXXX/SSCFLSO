#include "unit_test.h"
namespace RoundingUnitTest {
	bool run_tests() {
		try {
			std::cout << "Running Rounding Test 1..." << std::endl;
			test1();
			std::cout << "OK" << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}

	void test1() {
		SSCFLSO instance = Generator::load_instance("instances/i300/i300_17.plc");
		facility_vector res;
		Rounding::solve(instance, res, TWO_MINUTES);
		SSCFLSO instance = Generator::load_instance("instances/i300/i300_10.plc");
		Rounding::solve(instance, res, TWO_MINUTES);
		SSCFLSO instance = Generator::load_instance("instances/i300/i300_6.plc");
		Rounding::solve(instance, res, TWO_MINUTES);
	}
}
