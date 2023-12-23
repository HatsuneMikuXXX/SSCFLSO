#include "greedy.h"

solution greedy(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	solution open_facilities{};
	std::vector<int> closed_facilities = preprocess(instance);
	std::vector<int> tmp_facilities;
	std::vector<int>::iterator position;
	std::vector<std::pair<int, float>> costs;
	auto sort_by_second = [](std::pair<int, float> pair1, std::pair<int, float> pair2){ return pair1.second < pair2.second; };
	FLV.set_solution(open_facilities);
	// Open iteratively those facilities minimizes our money loss.
	while(!FLV.feasible() && !closed_facilities.empty()){
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