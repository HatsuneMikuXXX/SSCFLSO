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