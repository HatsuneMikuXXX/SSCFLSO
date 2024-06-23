#include "utility.h"

SolutionContainer::SolutionContainer(const facility_vector& root) : root(root), tail(root) {}

void SolutionContainer::add(const facility_vector& next) {
	assert(next.size() == this->root.size());
	int index = 0;
	bool no_change = true;
	asa::for_each(next, [this, &index, &no_change](const bool facility_is_open) {
		if (facility_is_open != this->tail[index]) {
			this->change_log.push_back((index + 1));
			this->tail[index++] = facility_is_open;
			no_change = false;
		}
		else {
			index++;
		}
	});
	if (!no_change) {
		change_log[change_log.size() - 1] *= -1;
	}
}

bool SolutionContainer::contains(const facility_vector& val) const {
	assert(val.size() == this->root.size());
	// Compute difference vector and hamming distance
	std::vector<bool> d(val.size());
	int hd = 0;

	int index = 0;
	asa::generate(d, [this, &val, &index, &hd]() -> bool {
		if (this->root[index] != val[index++]) {
			hd++;
			return true;
		}
		return false;
	});
	// Do the comparison
	if (hd == 0) {
		return true;
	}
	index = 0;
	while (index < this->change_log.size()) {
		int log_entry = this->change_log[index++];
		int true_index = (log_entry < 0) ? (-log_entry) - 1 : log_entry - 1;
		hd += d[true_index] ? -1 : 1;
		if (log_entry < 0 && hd == 0) {
			return true;
		}
		d[true_index] = !d[true_index];
	}
	return false;
}

std::vector<int> range(int n) {
	range_vector r(n);
	std::iota(std::begin(r), std::end(r), 0);
	return r;
}

void filter(facility_vector& facilities, const facility_predicate& predicate) {
	range_vector facility_range = range(facilities.size());
	asa::for_each(facility_range, [&facilities, &predicate](const int& facility_id) { facilities[facility_id] = facilities[facility_id] && predicate(facility_id); });
}

void inverse(std::vector<int>& output, const std::vector<int>& input) {
	output.clear();
	output.resize(input.size());
	int index = 0;
	asa::for_each(input, [&index, &output](const int val) {
		output[val] = index++;
	});
}

bool are_same(double a, double b){
	return fabs(a - b) < EPSILON;
}
bool are_same(const std::vector<double>& a, const std::vector<double>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	int index = 0;
	asa::for_each(a, [&b, &index](const int val) { 
		if (!are_same(val, b[index++])) {
			return false; 
		}
	});
	return true;
}

bool are_same(const std::vector<int>& a, const std::vector<int>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	int index = 0;
	asa::for_each(a, [&b, &index](const int val) {
		if (!are_same(val, b[index++])) {
			return false;
		}
		});
	return true;
}

bool are_same(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b) {
	if (b.size() != a.size()) {
		return false;
	}
	int index = 0;
	asa::for_each(a, [&b, &index](const std::vector<double>& val) {
		if (!are_same(val, b[index++])) {
			return false;
		}
		});
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