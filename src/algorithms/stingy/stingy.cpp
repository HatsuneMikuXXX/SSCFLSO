#include "stingy.h"

std::vector<facility_vector> remove_neighborhood(facility_vector& x);

facility_vector stingy(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	facility_vector solution = preprocess(instance);
	FLV.set_solution(solution);
	bool stuck_in_local_optima = false;
	while(!stuck_in_local_optima){
		stuck_in_local_optima = true;
		// Search neighborhood
		double current_value = FLV.value();
		std::vector<facility_vector> neighborhood = remove_neighborhood(solution);
		for (auto it = neighborhood.begin(); it != neighborhood.end(); it++) {
			FLV.set_solution(*it);
			if (FLV.feasible() && FLV.value() < current_value) {
				current_value = FLV.value();
				solution = *it; // For the next while-iteration
				stuck_in_local_optima = false;
			}
		}
		FLV.set_solution(solution);
	}
	return solution;
}

facility_vector biased_stingy(const SSCFLSO& instance) {
	Validator FLV = Validator(instance);
	facility_vector solution = preprocess(instance);
	FLV.set_solution(solution);
	bool stuck_in_local_optima = false;
	while (!stuck_in_local_optima) {
		stuck_in_local_optima = true;
		// Get neighborhood and split it into two 
		std::vector<facility_vector> neighborhood_prio1 = {};
		std::vector<facility_vector> neighborhood_prio2 = {};
		client_facility_assignment assignment = FLV.get_assignment();
		for (int j = 0; j < instance.facilities; j++) {
			if (solution[j] == 0) { continue; }
			facility_vector neighbor = solution;
			neighbor[j] = 0;
			if (contains(assignment, j)) {
				neighborhood_prio1.push_back(neighbor);
			}
			else {
				neighborhood_prio2.push_back(neighbor);
			}
		}
		// Search neighborhood 1 first
		bool stuck_in_neighborhood1 = true;
		double current_value = FLV.value();
		for (auto it = neighborhood_prio1.begin(); it != neighborhood_prio1.end(); it++) {
			FLV.set_solution(*it);
			if (FLV.feasible() && FLV.value() < current_value) {
				current_value = FLV.value();
				solution = *it; // For the next while-iteration
				stuck_in_local_optima = false;
				stuck_in_neighborhood1 = false;
			}
		}
		// Search neighborhood 2 if first search was fruitless
		if (stuck_in_neighborhood1) {
			for (auto it = neighborhood_prio2.begin(); it != neighborhood_prio2.end(); it++) {
				FLV.set_solution(*it);
				if (FLV.feasible() && FLV.value() < current_value) {
					current_value = FLV.value();
					solution = *it; // For the next while-iteration
					stuck_in_local_optima = false;
				}
			}
		}
		FLV.set_solution(solution);
	}
	return solution;
}