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
	Timer timer(3 * ONE_MINUTE);
	ReportResult dummy_report = ReportResult::dummy_report();
	solution_and_value SV = {facility_vector(instance.facilities, 0), -1};
	LocalSearch LocalSearchAlgorithmObject = LocalSearch(LocalSearch::PREPROCESS, LocalSearch::BEST);
	LocalSearchAlgorithmObject.solve(instance, SV, timer, dummy_report, false);
	if(!no_facility_is_open(SV.sol)){
		throw std::runtime_error("Solution space is empty but Local Search returned a feasible solution");
	}
	instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
	LocalSearchAlgorithmObject.solve(instance, SV, timer, dummy_report, false);
	if (!(asa::sum(SV.sol, 0) == 3 && SV.sol[2] == 1 && SV.sol[3] == 1)) {
		throw std::runtime_error("The returned solution should be of size 3 and contain the facilities 2 and 3.");
	}
}

