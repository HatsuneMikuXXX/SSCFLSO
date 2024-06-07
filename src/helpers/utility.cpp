#include "utility.h"

void filter(facility_vector& facilities, const facility_predicate& predicate) {
	std::vector<int> range(facilities.size());
	std::iota(std::begin(range), std::end(range), 0);
	std::for_each(std::begin(range), std::end(range), 
		[&facilities, &predicate](const int& facility_id) {facilities[facility_id] *= predicate(facility_id);});
}

bool are_same(double a, double b){
	return fabs(a - b) < EPSILON;
}
bool are_same(const std::vector<double>& a, const std::vector<double>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	for (int i = 0; i < a.size(); i++) {
		if (!are_same(a[i], b[i])) {
			return false;
		}
	}
	return true;
}

bool are_same(const std::vector<int>& a, const std::vector<int>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	for (int i = 0; i < a.size(); i++) {
		if (!are_same(a[i], b[i])) {
			return false;
		}
	}
	return true;
}

bool are_same(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	for (int i = 0; i < a.size(); i++) {
		if (!are_same(a[i], b[i])) {
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
	std::for_each(std::begin(v), std::end(v), [&res](const double& val) {res += val * val; });
	return sqrt(res);
}

std::string primitive_list_to_string(const std::vector<int>& container) {
	std::string msg = "[";
	std::for_each(std::begin(container), std::end(container), [&msg](const double& value) { msg += std::to_string(value) + ","; });
	msg.pop_back();
	msg += "]";
	return msg;
}

std::string primitive_list_to_string(const std::vector<double>& container) {
	std::string msg = "[";
	std::for_each(std::begin(container), std::end(container), [&msg](const double& value) { msg += std::to_string(value) + ","; });
	msg.pop_back();
	msg += "]";
	return msg;
}