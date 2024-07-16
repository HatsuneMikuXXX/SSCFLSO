#include "unit_test.h"

bool AlgorithmUnitTest::run_tests(){
	try{
		std::cout << "Running Algorithm Test 1. Improve a solution." << std::endl;
		test1();
		std::cout << "OK" << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void AlgorithmUnitTest::test1(){
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
	solution_and_value SV = {facility_vector(instance.facilities, 0), -1};
	facility_vector solution({1, 1, 0, 0, 0});
	Timer timer(ONE_MINUTE);
	ReportResult report = ReportResult::dummy_report();
	Algorithm::UPDATE_CODE expected_result;
	expected_result = Algorithm::improve_solution(instance, SV, solution, timer, report);
	assert(expected_result == Algorithm::UPDATE_CODE::TIMER_NOT_RUNNING);
	timer.start_timer();
	expected_result = Algorithm::improve_solution(instance, SV, solution, timer, report);
	assert(expected_result == Algorithm::UPDATE_CODE::IMPROVED);
	expected_result = Algorithm::improve_solution(instance, SV, solution, timer, report);
	assert(expected_result == Algorithm::UPDATE_CODE::NOT_IMPROVED);
	while (timer.in_time()) {}
	expected_result = Algorithm::improve_solution(instance, SV, solution, timer, report);
	assert(expected_result == Algorithm::UPDATE_CODE::TIMEOUT);
}
