#include "stingy.h"
void Stingy::solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) {
	auto start = start_timer();
	Validator FLV = Validator(instance);
	facility_vector solution = preprocess(instance);
	current_best = solution; // In case preprocessed solution is already at a local optima
	bool stuck_in_local_optima = false;
	while (!stuck_in_local_optima) {
		stuck_in_local_optima = true;
		// Search neighborhood
		FLV.set_solution(solution);
		double current_value = FLV.value();
		std::vector<facility_vector> neighborhood = remove_neighborhood(solution);
		for (auto neighbor = neighborhood.begin(); neighbor != neighborhood.end(); neighbor++) {
			FLV.set_solution(*neighbor);
			if (FLV.feasible() && FLV.value() < current_value) {
				current_value = FLV.value();
				solution = *neighbor; // For the next while-iteration
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
}

std::string Stingy::meta_information() {
	std::ifstream file(PATH + "stingy/stingy.txt");
	if (file) {
		std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		return content;
	}
	else {
		throw std::runtime_error("Stingy information file not found.");
	}
}
