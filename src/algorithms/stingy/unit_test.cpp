#include "unit_test.h"

bool StingyUnitTest::run_tests(){
	try{
		std::cout << "Running Stingy Test 1..." << std::endl;
		test1();
		std::cout << "OK " << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}
	
void StingyUnitTest::test1(){
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_infeasible0.plc");
	facility_vector res;
	Stingy StingyAlgorithmObject = Stingy();
	StingyAlgorithmObject.solve(instance, res, TWO_MINUTES);
	if(sum(res) > 0){
		throw std::runtime_error("Solution space is empty but Stingy returned a feasible solution");
	}
	instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
	StingyAlgorithmObject.solve(instance, res, TWO_MINUTES);
	if(!(sum(res) == 3 && res[2] == 1 && res[3] == 1)) {
		throw std::runtime_error("The returned solution should be of size 3 and contain the facilities 2 and 3.");
	}
}

