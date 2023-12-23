#include "preprocess.h"
std::vector<int> preprocess(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	std::vector<int> open_facilities = range(instance.facilities);
	std::vector<int> violating_facilities;
	std::vector<int>::iterator position;
	FLV.set_solution(open_facilities);
	while(!FLV.feasible() && !open_facilities.empty()){
		violating_facilities = FLV.violating_facilities();
		std::function<bool(int)> predicate = [violating_facilities](int facility){ return !contains(violating_facilities, facility); };
		filter(open_facilities, predicate);
		FLV.set_solution(open_facilities);
	}
	return open_facilities;
}