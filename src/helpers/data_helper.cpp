#include "data_helper.h"

std::vector<int> inverse(const std::vector<int> set){
	std::vector<int> res = std::vector<int>(set.size(), 0);
	int i = 0;
	for(auto it = set.begin(); it != set.end(); it++){
		res[*it] = i;
		i++;
	}
	return res;
}

std::vector<int> range(int n){
	std::vector<int> res{};
	for(int i = 0; i < n; i++){
		res.push_back(i);
	}
	return res;
}

bool areSame(double a, double b){
	return fabs(a - b) < EPSILON;
}
bool areSame(std::vector<double>& a, std::vector<double>& b) {
	if (b.size() < a.size()) {
		return false;
	}
	for (int i = 0; i < a.size(); i++) {
		if (!areSame(a[i], b[i])) {
			return false;
		}
	}
	return a.size() == b.size();
}
bool areSame(std::vector<std::vector<double>>& a, std::vector<std::vector<double>>& b) {
	if (b.size() < a.size()) {
		return false;
	}
	for (int i = 0; i < a.size(); i++) {
		if (!areSame(a[i], b[i])) {
			return false;
		}
	}
	return a.size() == b.size();
}

int sum(std::vector<int>& a) {
	return std::accumulate(a.begin(), a.end(), 0);
}

double sum(std::vector<double>& a) {
	return std::accumulate(a.begin(), a.end(), 0);
}

int hamming_distance(facility_vector& a, facility_vector& b) {
	assert(a.size() == b.size());
	facility_vector c = facility_vector(a.size(), 0);
	std::transform(a.begin(), a.end(), b.begin(), c.begin(), std::bit_xor<int>());
	return std::accumulate(c.begin(), c.end(), 0);
}

void filter(facility_vector& facilities, facility_predicate& predicate) {
	for (int j = 0; j < facilities.size(); j++) {
		if (facilities[j] == 0) { continue; }
		else if (facilities[j] != 1) { assert(false); }
		facilities[j] = predicate(j);
	}
}

std::string solution_to_string(const std::vector<int>& container){
	std::string res = "[";
	for(int entry : container){
		res += std::to_string(entry) + ", ";
	}
	res = res.substr(0, res.size() - 2) + "]";
	return res;
}

std::string solution_to_string(const std::vector<double>& container){
	std::string res = "[";
	for(float entry : container){
		res += std::to_string(entry) + ", ";
	}
	res = res.substr(0, res.size() - 2) + "]";
	return res;
}