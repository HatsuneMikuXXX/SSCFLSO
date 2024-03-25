#include "unit_test.h"

bool SemiLagrangianRelaxationUnitTest::run_tests() {
	try {
		std::cout << "Running SLR Test 1..." << std::endl;
		test1();
		std::cout << "OK" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void SemiLagrangianRelaxationUnitTest::test1() {
	SSCFLSO instance = Generator::load_instance("instances/i300/i300_17.plc", false);
	//SSCFLSO instance = Generator::load_instance("instances/custom/example.plc");
	facility_vector res;
	SemiLagrangianRelaxation SLRAlgorithmObject = SemiLagrangianRelaxation();
	SLRAlgorithmObject.solve(instance, res, TWO_MINUTES);
	Validator FLV = Validator(instance);
	FLV.set_solution(res);
	std::cout << FLV.feasible() << std::endl;
	for (auto it = res.begin(); it != res.end(); it++) {
		std::cout << *it << ", ";
	}
	std::cout << std::endl;
}

