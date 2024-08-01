#include "utility.h"

std::vector<int> range(size_t n) {
	range_vector r(n);
	std::iota(std::begin(r), std::end(r), 0);
	return r;
}

std::vector<int> range(int n) {
	if (n < 0) {
		n = 0;
	}
	range_vector r(n);
	std::iota(std::begin(r), std::end(r), 0);
	return r;
}

void filter(std::vector<bool>& container, const std::function<bool(int)>& predicate) {
	range_vector the_range = range(container.size());
	asa::for_each(the_range, [&container, &predicate](const bool& elem_id) { container[elem_id] = container[elem_id] && predicate(elem_id); });
}

void inverse(std::vector<int>& output, const std::vector<int>& input) {
	output.clear();
	output.resize(input.size());
	int index = 0;
	asa::for_each(input, [&index, &output](const int val) {
		output[val] = index++;
	});
}

bool no_facility_is_open(const std::vector<bool>& container) {
	return !asa::any_of(container, [](const bool val) -> bool { return val; });
}

bool are_same(double a, double b){
	return fabs(a - b) < EPSILON;
}
bool are_same(const std::vector<double>& a, const std::vector<double>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	range_vector indices = range(a.size());
	return asa::all_of(indices, [&a, &b](const int index) -> bool { return are_same(a[index], b[index]);});
}

bool are_same(const std::vector<int>& a, const std::vector<int>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	range_vector indices = range(a.size());
	return asa::all_of(indices, [&a, &b](const int index) -> bool { return are_same(a[index], b[index]); });
}

bool are_same(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	range_vector indices = range(a.size());
	return asa::all_of(indices, [&a, &b](const int index) -> bool { return are_same(a[index], b[index]); });
}

double magnitude(const std::vector<double>& v) {
	double res = 0;
	asa::for_each(v, [&res](const double& val) {res += val * val; });
	return sqrt(res);
}

std::string primitive_list_to_string(const std::vector<bool>& container) {
	std::string msg = "[";
	asa::for_each(container, [&msg](const bool value) { msg += std::to_string(int(value)) + ","; });
	msg.pop_back();
	msg += "]";
	return msg;
}

std::string primitive_list_to_string(const std::vector<int>& container) {
	std::string msg = "[";
	asa::for_each(container, [&msg](const int value) { msg += std::to_string(value) + ","; });
	msg.pop_back();
	msg += "]";
	return msg;
}

std::string primitive_list_to_string(const std::vector<double>& container) {
	std::string msg = "[";
	asa::for_each(container, [&msg](const double value) { msg += std::to_string(value) + ","; });
	msg.pop_back();
	msg += "]";
	return msg;
}