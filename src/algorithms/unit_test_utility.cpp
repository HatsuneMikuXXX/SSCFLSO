#include "unit_test_utility.h"

void expect_solution(Algorithm* algo, const SSCFLSO& instance, const int time_limit, const facility_vector& expected_solution) {
	if (algo == NULL) {
		throw std::runtime_error("Algorithm pointer is NULL");
	}
	Timer timer(time_limit);
	ReportResult dummy_report = ReportResult::dummy_report();
	solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
	timer.start_timer();
	algo->solve(instance, SV, timer, dummy_report, false);
	if (SV.sol != expected_solution) {
		std::cout << "Expected: " << primitive_list_to_string(expected_solution) << std::endl;
		std::cout << "Computed: " << primitive_list_to_string(SV.sol) << std::endl;
		throw std::runtime_error("Returned solution is unexpected.");
	}
}

void check_timelimit_enforcement(Algorithm* algo, const SSCFLSO& instance) {
	if (algo == NULL) {
		throw std::runtime_error("Algorithm pointer is NULL");
	}
	Timer timer(2 * ONE_MINUTE);
	ReportResult dummy_report = ReportResult::dummy_report();
	solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
	timer.start_timer();
	algo->solve(instance, SV, timer, dummy_report, false);
}

void print_current_system_time() {
	std::time_t tmp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::cout << "Time stamp: " << std::ctime(&tmp) << std::endl;
}