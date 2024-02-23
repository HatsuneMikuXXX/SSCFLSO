#include "data_helper.h"

std::vector<int> inverse(const std::vector<int>& index_set){
	std::vector<int> res = std::vector<int>(index_set.size(), 0);
	int i = 0;
	for(auto it = index_set.begin(); it != index_set.end(); it++){
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

void filter(facility_vector& facilities, const facility_predicate& predicate) {
	for (int j = 0; j < facilities.size(); j++) {
		if (facilities[j] == 0) { continue; }
		else if (facilities[j] != 1) { assert(false); }
		facilities[j] = predicate(j);
	}
}

bool areSame(double a, double b){
	return fabs(a - b) < EPSILON;
}
bool areSame(const std::vector<double>& a, const std::vector<double>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	for (int i = 0; i < a.size(); i++) {
		if (!areSame(a[i], b[i])) {
			return false;
		}
	}
	return true;
}
bool areSame(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	for (int i = 0; i < a.size(); i++) {
		if (!areSame(a[i], b[i])) {
			return false;
		}
	}
	return true;
}

int sum(const std::vector<int>& a) {
	return std::accumulate(a.begin(), a.end(), 0);
}

double sum(const std::vector<double>& a) {
	return std::accumulate(a.begin(), a.end(), 0);
}

int hamming_distance(facility_vector& a, facility_vector& b) {
	assert(a.size() == b.size());
	facility_vector c = facility_vector(a.size(), 0);
	std::transform(a.begin(), a.end(), b.begin(), c.begin(), std::bit_xor<int>());
	return std::accumulate(c.begin(), c.end(), 0);
}

double magnitude(const std::vector<double>& v) {
	double res = 0;
	for (auto it = v.begin(); it != v.end(); it++) {
		res += (*it) * (*it);
	}
	return sqrt(res);
}