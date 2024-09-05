#include "benchmark.h"

void run(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, const int timelimit_in_milliseconds, Algorithm* const algo_ptr, const bool gurobi_afterwards) {
	solution_and_value SV{ facility_vector(instance.facilities, 0) , -1};
	Validator FLV(instance);
	Timer timer = Timer();
	const std::string algo_name = algo_ptr->name();
	ReportResult report(save_to_path, instance, instance_name, algo_name, gurobi_afterwards);
	timer.start_timer();
	try {
		algo_ptr->solve(instance, SV, timer, report, gurobi_afterwards);
	}
	catch (...) {
		std::cout << algo_ptr->name() << " on " << instance_name << " caused an exception" << std::endl;
	}
}
