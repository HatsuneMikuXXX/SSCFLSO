#include "utility.h"

SolutionContainer::SolutionContainer(const facility_vector& root) : root(root), tail(root) {}

void SolutionContainer::add(const facility_vector& next) {
	assert(next.size() == root.size());
	int index = 0;
	bool no_change = true;
	asa::for_each(next, [this, &index, &no_change](const bool facility_is_open) {
		if (facility_is_open != tail[index]) {
			change_log.push_back((index + 1));
			tail[index++] = facility_is_open;
			no_change = false;
		}
		else {
			index++;
		}
	});
	if (!no_change) {
		change_log.back() *= -1;
	}
}

bool SolutionContainer::contains(const facility_vector& val) const {
	assert(val.size() == root.size());
	// Compute difference vector and hamming distance
	std::vector<bool> d(val.size());
	int hd = 0;

	int index = 0;
	asa::generate(d, [this, &val, &index, &hd]() -> bool {
		if (root[index] != val[index++]) {
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
	while (index < change_log.size()) {
		int log_entry = change_log[index++];
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

bool no_facility_is_open(const facility_vector& solution) {
	return !asa::any_of(solution, [](const bool is_open) -> bool { return is_open; });
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

int hamming_distance(facility_vector& a, facility_vector& b) {
	assert(a.size() == b.size());
	facility_vector c = facility_vector(a.size(), 0);
	asa::transform(a, b, c, std::bit_xor<int>());
	return asa::sum(c, 0);
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

void expect_solution(Algorithm* algo, const SSCFLSO& instance, const int time_limit, const facility_vector& expected_solution) {
	Timer timer(time_limit);
	ReportResult dummy_report = ReportResult::dummy_report();
	solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
	algo->solve(instance, SV, timer, dummy_report, false);
	if (SV.sol != expected_solution) {
		throw std::runtime_error("Returned solution is unexpected.");
	}
}