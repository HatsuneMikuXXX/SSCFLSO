#include "local_search.h"

facility_vector local_search(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	facility_vector no_unnecessary_facilities = preprocess(instance);
	facility_vector solution = no_unnecessary_facilities;
	std::vector<facility_vector> neighborhood;
	std::vector<facility_vector> tmp;
	bool stuck_in_local_optima = false;
	while(!stuck_in_local_optima){
		stuck_in_local_optima = true;
		// Collect neighborhood
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
				solution = *neighbor;
				current_value = FLV.value();
				stuck_in_local_optima = false;
			}
		}
	}
	return solution;
}