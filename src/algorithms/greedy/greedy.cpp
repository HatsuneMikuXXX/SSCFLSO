#include "greedy.h"

// Utility is sorted in ascending order
std::vector<std::pair<int, double>> utility(const SSCFLSO& instance, const facility_vector& no_unnecessary_facilities, bool popularity);

facility_vector greedy_with_popularity(const SSCFLSO& instance){
	// Compute utilities
	std::vector<int> no_unnecessary_facilities = preprocess(instance);
	if (sum(no_unnecessary_facilities) == 0) {
		return no_unnecessary_facilities;
	}
	std::vector<std::pair<int, double>> utilities = utility(instance, no_unnecessary_facilities, true);

	// Greedy
	Validator FLV = Validator(instance);
	facility_vector solution = facility_vector(instance.facilities, 0);
	FLV.set_solution(solution);
	int index = instance.facilities - 1;
	while (!FLV.feasible()) {
		int facility = (utilities[index]).first;
		assert(no_unnecessary_facilities[facility] == 1);
		solution[facility] = 1;
		FLV.set_solution(solution);
		index--;
	}
	FLV.drop_empty_facilities();
	solution = FLV.get_solution();
	return solution;
}

facility_vector greedy_without_popularity(const SSCFLSO& instance) {
	// Compute utilities
	std::vector<int> no_unnecessary_facilities = preprocess(instance);
	if (sum(no_unnecessary_facilities) == 0) {
		return no_unnecessary_facilities;
	}
	std::vector<std::pair<int, double>> utilities = utility(instance, no_unnecessary_facilities, false);

	// Greedy
	Validator FLV = Validator(instance);
	facility_vector solution = facility_vector(instance.facilities, 0);
	FLV.set_solution(solution);
	int index = instance.facilities - 1;
	while (!FLV.feasible()) {
		int facility = (utilities[index]).first;
		assert(no_unnecessary_facilities[facility] == 1);
		solution[facility] = 1;
		FLV.set_solution(solution);
		index--;
	}
	FLV.drop_empty_facilities();
	solution = FLV.get_solution();
	return solution;
}

std::vector<std::pair<int, double>> utility(const SSCFLSO& instance, const facility_vector& no_unnecessary_facilities, bool popularity) {
	std::vector<std::pair<int, double>> utilities = {};
	// Collect capacities of no-unnecessary facilities
	std::vector<double> capacities = {};
	std::function<bool(double, double)> is_less = [](double a, double b) { return a < b; };
	for (int j = 0; j < instance.facilities; j++) {
		if (no_unnecessary_facilities[j] == 0) { continue; }
		bisect_insert(capacities, instance.capacities[j], is_less);
	}
	// Compute minimum amount of facilities required
	int min = 0;
	double tot_demand = sum(instance.demands);
	double min_capacity = 0;
	for (auto it = capacities.begin(); it != capacities.end() && min_capacity < tot_demand; it++) {
		min += 1;
		min_capacity += *it;
	}
	// Compute rankings without unnecessary facilities
	std::vector<std::map<int, int>> rankings;
	for (int i = 0; i < instance.clients; i++) {
		std::map<int, int> ranking = {};
		int rank = 0;
		for (auto it = instance.preferences[i].begin(); it != instance.preferences[i].end(); it++) {
			if (no_unnecessary_facilities[*it] == 1) {
				ranking[*it] = rank;
				rank++;
			}
		}
		rankings.push_back(ranking);
	}
	// Compute utilities
	int m = sum(no_unnecessary_facilities);
	for (int j = 0; j < instance.facilities; j++) {
		if (no_unnecessary_facilities[j] == 0) {
			bisect_insert(utilities, std::pair<int, double>(j, 0), sort_by_second);
		}
		else {
			double utility = instance.facility_costs[j];
			double popularity_factor;
			for (int i = 0; i < instance.clients; i++) {
				if (rankings[i][j] > m - min) { continue; }
				popularity_factor = 1. - (((double)rankings[i][j])/(m - min + 1));
				utility += (popularity) ? (instance.distribution_costs[j][i] * popularity_factor) : instance.distribution_costs[j][i];
			}
			utility = (utility == 0) ? DBL_MAX : instance.capacities[j] / utility;
			bisect_insert(utilities, std::pair<int, double>(j, utility), sort_by_second);
		}
	}
	return utilities;
}

facility_vector UBH(const SSCFLSO& instance) {
	// Compute utilities
	facility_vector no_unnecessary_facilities = preprocess(instance);
	if (sum(no_unnecessary_facilities) == 0) {
		return no_unnecessary_facilities;
	}
	// Greedy
	Validator FLV = Validator(instance);
	facility_vector solution = facility_vector(instance.facilities, 0);
	FLV.set_solution(solution);
	int index = instance.facilities - 1;
	while (!FLV.feasible()) {
		std::vector<facility_vector> neighborhood = add_neighborhood(solution, no_unnecessary_facilities);
		double TC_best = DBL_MAX;
		for (auto it = neighborhood.begin(); it != neighborhood.end(); it++) {
			FLV.set_solution(*it);
			double TC_current = FLV.value();
			for (int j = 0; j < instance.facilities; j++) {
				if ((*it)[j] == 0) { continue; }
				TC_current -= instance.facility_costs[j];
			}
			if (TC_current < TC_best) {
				TC_current = TC_best;
				solution = *it;
			}
		}
		FLV.set_solution(solution);
	}
	FLV.drop_empty_facilities();
	solution = FLV.get_solution();
	return solution;
}