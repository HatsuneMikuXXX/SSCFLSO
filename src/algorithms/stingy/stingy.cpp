#include "stingy.h"

solution stingy(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	solution open_facilities = preprocess(instance);
	std::vector<int> tmp_facilities;
	std::vector<int>::iterator position;
	std::vector<std::pair<int, float>> costs;
	auto sort_by_second = [](std::pair<int, float> pair1, std::pair<int, float> pair2){ return pair1.second < pair2.second; };
	while(true){
		// Determine costs
		costs = {};
		for(int facility : open_facilities){
			tmp_facilities = open_facilities;
			position = find(tmp_facilities.begin(), tmp_facilities.end(), facility);
			tmp_facilities.erase(position);
			FLV.set_solution(tmp_facilities);
			if(FLV.feasible()){
				costs.push_back(std::pair<int, float>(facility, FLV.value()));
			}
		}
		if(costs.empty()){ break; }
		// Choose best
		sort(costs.begin(), costs.end(), sort_by_second);
		position = find(open_facilities.begin(), open_facilities.end(), costs[0].first);
		open_facilities.erase(position);
	}
	return open_facilities;
}