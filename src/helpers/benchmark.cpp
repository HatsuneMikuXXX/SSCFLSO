#include "benchmark.h"

void run(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, const int timelimit_in_milliseconds, Algorithm* const algo_ptr, const bool gurobi_afterwards) {
	solution_and_value SV{ facility_vector(instance.facilities, 0) , -1};
	Validator FLV(instance);
	Timer timer(timelimit_in_milliseconds);
	ReportResult report(instance, instance_name, timelimit_in_milliseconds, algo_ptr->name(), gurobi_afterwards);
	timer.start_timer();
	algo_ptr->solve(instance, SV, timer, report, gurobi_afterwards);
	report.finishUp(save_to_path);
}
