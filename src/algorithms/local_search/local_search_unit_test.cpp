#include "local_search_unit_test.h"

bool LocalSearchUnitTest::run_tests(){
	try{
		std::cout << "Running Local Search Test 1. Expect a certain solution." << std::endl;
		test1();
		std::cout << "OK " << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << "TEST FAILED!" << std::endl;
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}
	
void LocalSearchUnitTest::test1(){
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc", true);
	LocalSearch local_search = LocalSearch(LocalSearch::PREPROCESS, LocalSearch::BEST);
	expect_solution(&local_search, instance, 3 * ONE_MINUTE, facility_vector({0, 1, 1, 1, 0}));
}

