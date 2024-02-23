#include "greedy.h"

class Greedy : public Algorithm {
	static void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) {
		auto start = start_timer();
		// Compute utilities
		std::vector<int> no_unnecessary_facilities = preprocess(instance);
		if (sum(no_unnecessary_facilities) == 0) {
			current_best = no_unnecessary_facilities;
			return;
		}
		std::vector<std::pair<int, double>> utilities = utility(instance, no_unnecessary_facilities);
		// Greedy
		Validator FLV = Validator(instance);
		facility_vector solution = facility_vector(instance.facilities, 0);
		FLV.set_solution(solution);
		int index = instance.facilities - 1; // Utilities are sorted in ascending order
		while (!FLV.feasible()) {
			int facility = (utilities[index]).first;
			assert(no_unnecessary_facilities[facility] == 1);
			solution[facility] = 1;
			if (!within_time_limit(start, time_limit)) { return; }
			current_best = solution;
			FLV.set_solution(solution);
			index--;
		}
		FLV.drop_empty_facilities();
		if (!within_time_limit(start, time_limit)) { return; }
		current_best = FLV.get_solution();
	}

	static std::vector<std::pair<int, double>> utility(const SSCFLSO& instance, const facility_vector& no_unnecessary_facilities) {
		std::vector<std::pair<int, double>> utilities = {};
		// Compute utilities
		for (int j = 0; j < instance.facilities; j++) {
			if (no_unnecessary_facilities[j] == 0) {
				bisect_insert(utilities, std::pair<int, double>(j, -1), sort_by_second);
			}
			else {
				double denominator = (instance.facility_costs[j] + sum(instance.distribution_costs[j]));
				double utility = (denominator == 0) ? DBL_MAX : instance.capacities[j] / denominator;
				bisect_insert(utilities, std::pair<int, double>(j, utility), sort_by_second);
			}
		}
		return utilities;
	}

	static std::string meta_information() {
		std::ifstream file(PATH + "greedy/greedy.txt");
		if (file) {
			std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
			return content;
		}
		else {
			throw std::runtime_error("Greedy information file not found.");
		}
	}
};