#include "greedy.h"

facility_vector greedy(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	facility_vector solution = facility_vector(instance.facilities, 0);

	std::vector<int> no_unnecessary_facilities = preprocess(instance);
	std::vector<int> tmp_facilities;
	std::vector<int>::iterator position;
	std::vector<std::pair<int, float>> costs;
	auto sort_by_second = [](std::pair<int, float> pair1, std::pair<int, float> pair2){ return pair1.second < pair2.second; };

	FLV.set_solution(solution);
	// Open iteratively those facilities minimizes our money loss.
	while(!FLV.feasible()){
		// Search neighborhood
		double current_value = FLV.value();
		std::vector<facility_vector> neighborhood = add_neighborhood(solution, no_unnecessary_facilities);
		for (auto it = neighborhood.begin(); it != neighborhood.end(); it++) {
			FLV.set_solution(*it);
			if (FLV.feasible() && FLV.value() < current_value) {
				current_value = FLV.value();
				solution = *it; // For the next while-iteration
				stuck_in_local_optima = false;
			}
		}
		FLV.set_solution(solution);

		costs = {};
		for(int facility : closed_facilities){
			tmp_facilities = open_facilities;
			tmp_facilities.push_back(facility);
			FLV.set_solution(tmp_facilities);
			costs.push_back(std::pair<int, float>(facility, FLV.value()));
		}
		sort(costs.begin(), costs.end(), sort_by_second);
		open_facilities.push_back(costs[0].first);
		position = find(closed_facilities.begin(), closed_facilities.end(), costs[0].first);
		closed_facilities.erase(position);
		FLV.set_solution(open_facilities);
	}
	return open_facilities;
}