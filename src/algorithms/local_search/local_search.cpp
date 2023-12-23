#include "local_search.h"

solution local_search(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	solution open_facilities = preprocess(instance);
	std::vector<int> closed_facilities{};
	FLV.set_solution(open_facilities);
	int current_value = FLV.value();
	while(true){
		if(add(FLV, open_facilities, closed_facilities, current_value)){ continue; }
		if(remove(FLV, open_facilities, closed_facilities, current_value)){ continue; }
		if(swap(FLV, open_facilities, closed_facilities, current_value)){ continue; }
		break;
	}
	return open_facilities;
}

bool add(Validator& FLV, solution& open_facilities, std::vector<int>& closed_facilities, int& current_value){
	std::vector<int> tmp;
	std::vector<int>::iterator position;
	for(int facility : closed_facilities){
		tmp = open_facilities;
		tmp.push_back(facility);
		FLV.set_solution(tmp);
		if(FLV.feasible() && FLV.value() < current_value){
			open_facilities.push_back(facility);
			position = find(closed_facilities.begin(), closed_facilities.end(), facility);
			closed_facilities.erase(position);
			current_value = FLV.value();
			return true;
		}
	}
	return false;
}

bool remove(Validator& FLV, solution& open_facilities, std::vector<int>& closed_facilities, int& current_value){
	std::vector<int> tmp;
	std::vector<int>::iterator position;
	for(int facility : open_facilities){
		tmp = open_facilities;
		position = find(tmp.begin(), tmp.end(), facility);
		tmp.erase(position);
		FLV.set_solution(tmp);
		if(FLV.feasible() && FLV.value() < current_value){
			closed_facilities.push_back(facility);
			position = find(open_facilities.begin(), open_facilities.end(), facility);
			open_facilities.erase(position);
			current_value = FLV.value();
			return true;
		}
	}
	return false;
}

bool swap(Validator& FLV, solution& open_facilities, std::vector<int>& closed_facilities, int& current_value){
	std::vector<int> tmp;
	std::vector<int>::iterator position;
	for(int facility_add : closed_facilities){
		tmp = open_facilities;
		tmp.push_back(facility_add);
		for(int facility_rem : open_facilities){
			position = find(tmp.begin(), tmp.end(), facility_rem);
			tmp.erase(position);
			FLV.set_solution(tmp);
			if(FLV.feasible() && FLV.value() < current_value){
				open_facilities.push_back(facility_add);
				position = find(open_facilities.begin(), open_facilities.end(), facility_rem);
				open_facilities.erase(position);
				closed_facilities.push_back(facility_rem);
				position = find(closed_facilities.begin(), closed_facilities.end(), facility_add);
				open_facilities.erase(position);
				current_value = FLV.value();
				return true;
			}
		}	
	}
	return false;
}