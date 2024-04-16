#include "unit_test.h"

bool LocalSearchUnitTest::run_tests(){
	try{
		std::cout << "Running Local Search Test 1..." << std::endl;
		test1();
		std::cout << "OK " << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}
	
void LocalSearchUnitTest::test1(){
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_infeasible0.plc");
	facility_vector res;
	LocalSearch LocalSearchAlgorithmObject = LocalSearch();
	LocalSearchAlgorithmObject.solve(instance, res, TWO_MINUTES, false);
	if(sum(res) > 0){
		throw std::runtime_error("Solution space is empty but Local Search returned a feasible solution");
	}
	instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
	LocalSearchAlgorithmObject.solve(instance, res, TWO_MINUTES, false);
	if (!(sum(res) == 3 && res[2] == 1 && res[3] == 1)) {
		throw std::runtime_error("The returned solution should be of size 3 and contain the facilities 2 and 3.");
	}
}

