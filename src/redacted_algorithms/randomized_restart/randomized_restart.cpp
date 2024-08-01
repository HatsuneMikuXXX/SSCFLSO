/*
#include "randomized_restart.h"
#include "../../helpers/random.h"

void RandomizedRestart::solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards) {
	auto start = start_timer();
	facility_vector no_unnecessary_facilities = preprocess(instance);
	if (sum(no_unnecessary_facilities) == 0) {
		return;
	}
	// We don't report back the preprocessed solution because we want to see whether this algorithm fails or not
	Validator FLV = Validator(instance);
	std::vector<facility_vector> known_feasible_solutions = std::vector<facility_vector>();
	known_feasible_solutions.push_back(no_unnecessary_facilities);
	double best_value = DBL_MAX;
	bool foundAtLeastOne = false;
	
	do {
		if (!within_time_limit(start, time_limit) ){ return; }
		// Guess initial feasible solution
		facility_vector randomized_solution = facility_vector(instance.facilities, 0);
		for (int j = 0; j < instance.facilities; j++) {
			if (no_unnecessary_facilities[j] == 0) {
				continue;
			}
			if (flip()) {
				randomized_solution[j] = 1;
			}
		}
		FLV.set_solution(randomized_solution);
		if (!FLV.feasible()) {
			continue;
		}
		else {
			if (!foundAtLeastOne) {
				std::cout << "Found at least one!" << std::endl;
				foundAtLeastOne = true;
			}
			if (FLV.value() < best_value) {
				if (within_time_limit(start, time_limit)) {
					current_best = FLV.get_solution();
				}
			}
		}
		if (rr_contains(known_feasible_solutions, randomized_solution)) {
			continue;
		}
		else {
			known_feasible_solutions.push_back(randomized_solution);
		}
		// Local Search
		{
			facility_vector solution = randomized_solution;
			std::vector<facility_vector> neighborhood;
			std::vector<facility_vector> tmp; // Collect subneighborhoods and add them to neighborhood
			bool stuck_in_local_optima = false;
			while (!stuck_in_local_optima) {
				stuck_in_local_optima = true;
				// Collect neighborhoods
				neighborhood = {};
				tmp = add_neighborhood(solution, no_unnecessary_facilities);
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
						// We have already seen this solution
						if (rr_contains(known_feasible_solutions, *neighbor)) {
							break;
						}
						else {
							known_feasible_solutions.push_back(*neighbor);
							stuck_in_local_optima = false;
							solution = *neighbor;
						}
						// Update
						if (FLV.value() < best_value) {
							best_value = FLV.value();
							if (within_time_limit(start, time_limit)) {
								current_best = FLV.get_solution();
							}
							break;
						}
						break;
					}
				}
			}
		}
	} while (within_time_limit(start, time_limit));
}

bool RandomizedRestart::rr_contains(const std::vector<facility_vector>& container, const facility_vector& x) {
	for (auto it = container.begin(); it != container.end(); it++) {
		if (areSame(*it, x)) {
			return true;
		}
	}
	return false;
}

std::string RandomizedRestart::meta_information() {
	return randomized_restart_info;
}
*/