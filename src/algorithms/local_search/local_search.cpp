#include "local_search.h"

void LocalSearch::solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards) {
	auto start = start_timer();
	Validator FLV = Validator(instance);
	facility_vector no_unnecessary_facilities = preprocess(instance);
	facility_vector solution = no_unnecessary_facilities;
	std::vector<facility_vector> neighborhood;
	std::vector<facility_vector> tmp; // Collect subneighborhoods and add them to neighborhood
	bool stuck_in_local_optima = false;
	while (!stuck_in_local_optima) {
		stuck_in_local_optima = true;
		// Collect neighborhoods
		neighborhood = {};
		tmp = swap_neighborhood(solution, no_unnecessary_facilities);
		neighborhood.insert(neighborhood.begin(), tmp.begin(), tmp.end());
		tmp = remove_neighborhood(solution);
		neighborhood.insert(neighborhood.begin(), tmp.begin(), tmp.end());
		// Conduct search
		FLV.set_solution(solution);
		double current_value = FLV.value();
		for (auto neighbor = neighborhood.begin(); neighbor != neighborhood.end(); neighbor++) {
			FLV.set_solution(*neighbor);
			if (FLV.feasible() && FLV.value() < current_value) {
				solution = *neighbor;
				current_value = FLV.value();
				stuck_in_local_optima = false;
			}
		}
		if (!within_time_limit(start, time_limit)) {
			return;
		}
		else {
			current_best = solution;
		}
	}
	if (gurobi_afterwards) {
		auto remaining = time_limit - get_elapsed_time_ms(start);
		if (remaining.count() < GUROBI_TIME_BUFFER) {
			return;
		}
		solution = solve_with_gurobi(instance, remaining, current_best);

		if (within_time_limit(start, time_limit)) {
			std::cout << "Gurobi Afterwards Successful" << std::endl;
			current_best = solution;
		}
	}
	return;
}

std::string LocalSearch::meta_information() {
	return local_search_info;
}
