#include "data_helper.h"

std::vector<int> range(int n){
	std::vector<int> res{};
	for(int i = 0; i < n; i++){
		res.push_back(i);
	}
	return res;
}

bool areSame(float a, float b){
	return fabs(a - b) < EPSILON;
}

std::string solution_to_string(const std::vector<int>& container){
	std::string res = "[";
	for(int entry : container){
		res += std::to_string(entry) + ", ";
	}
	res = res.substr(0, res.size() - 2) + "]";
	return res;
}

std::string solution_to_string(const std::vector<float>& container){
	std::string res = "[";
	for(float entry : container){
		res += std::to_string(entry) + ", ";
	}
	res = res.substr(0, res.size() - 2) + "]";
	return res;
}