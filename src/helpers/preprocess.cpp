#include "preprocess.h"

/**
 * Removes unnecessary facilities which are:
 * - Type I: Facilities that when opened always cause the solution to be infeasible
 * - Type II: Facilities that cannot even serve one client
*/

std::vector<int> preprocess(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	std::vector<int> open_facilities = range(instance.facilities);
	std::vector<int> violating_facilities;
	std::vector<int>::iterator position;
	FLV.set_solution(open_facilities);
	// Remove facilities of Type I
	while(!FLV.feasible() && !open_facilities.empty()){
		violating_facilities = FLV.violating_facilities();
		std::function<bool(int)> predicate = [violating_facilities](int facility){ return !contains(violating_facilities, facility); };
		filter(open_facilities, predicate);
		FLV.set_solution(open_facilities);
	}
	// Remove facilities of Type II
	float minimum_demand = *min_element(instance.demands.begin(), instance.demands.end());
	std::vector<float> capacities = instance.capacities;
	std::function<bool(int)> predicate = [capacities, minimum_demand](int facility){ return capacities[facility] >= minimum_demand; };
	filter(open_facilities, predicate);
	return open_facilities;
}