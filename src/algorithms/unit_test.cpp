#include "unit_test.h"

bool PreprocessUnitTest::run_tests(){
	try{
		std::cout << "Running Preprocess Test 1..." << std::endl;
		test1();
		std::cout << "OK" << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void PreprocessUnitTest::test1(){
	facility_vector check = facility_vector(100, 0);
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_infeasible1.plc");
	Preprocess preprocess = Preprocess();
	if(preprocess.preprocess(instance) != check){
		throw std::runtime_error("Solution space is empty but a feasible solution has still been found.");
	}
	check[69] = 1;
	instance = Generator::load_instance("instances/unit_tests/unit_test_only69.plc");
	if(preprocess.preprocess(instance) != check){
		throw std::runtime_error("Only facility 69 can be opened but the returned solution is different.");
	}
	instance = Generator::load_instance("instances/unit_tests/unit_test_preprocess.plc");
	check = preprocess.preprocess(instance);
	if(check[0] == 1) {
		throw std::runtime_error("Facility 0 is unnecessary but still included.");
	}
}
