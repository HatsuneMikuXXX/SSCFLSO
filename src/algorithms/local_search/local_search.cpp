#include "local_search.h"

void LocalSearch::solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) {
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
		tmp = add_neighborhood(solution, no_unnecessary_facilities); // I think this is not necessary, given the start
		neighborhood.insert(neighborhood.begin(), tmp.begin(), tmp.end());
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
				break;
			}
		}
		if (!within_time_limit(start, time_limit)) {
			return;
		}
		else {
			current_best = solution;
		}
	}
	return;
}

std::string LocalSearch::meta_information() {
	std::ifstream file(PATH + "local_search/local_search.txt");
	if (file) {
		std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		return content;
	}
	else {
		throw std::runtime_error("Local search information file not found.");
	}
}
