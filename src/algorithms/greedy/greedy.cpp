#include "greedy.h"

void Greedy::solve(const SSCFLSO& instance, solution_and_value& SAV, Timer& timer, const bool gurobi_afterwards) {
	// preprocess
	Preprocess preprocess_algorithm = Preprocess();
	facility_vector preprocessed_solution;
	solution_and_value preprocessed_SAV = { preprocessed_solution, -1 };
	preprocess_algorithm.solve(instance, preprocessed_SAV, timer, false);
	if (is_empty(preprocessed_SAV.sol)) { return; }

	// Compute utilities
	std::vector<std::pair<int, double>> utilities = {};
	{
		for (int j = 0; j < instance.facilities; j++) {
			if (preprocessed_SAV.sol[j] == 0) {
				bisect_insert(utilities, std::pair<int, double>(j, -1), sort_by_second);
			}
			else {
				double total_cost = (instance.facility_costs[j] + sum(instance.distribution_costs[j]));
				double utility = (total_cost == 0) ? DBL_MAX : instance.capacities[j] / total_cost;
				bisect_insert(utilities, std::pair<int, double>(j, utility), sort_by_second);
			}
		}
	}

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

	// Drop empty facilities afterwards
	FLV.drop_empty_facilities();
	if (!within_time_limit(start, time_limit)) { return; }

	// Apply gurobi afterwards
	current_best = FLV.get_solution();
	if (gurobi_afterwards) {
		auto remaining = time_limit - get_elapsed_time_ms(start);
		if (remaining.count() < GUROBI_TIME_BUFFER) {
			return;
		}
		solution = solve_with_gurobi(instance, remaining, current_best);

		if (within_time_limit(start, time_limit)) {
			std::cout << "Gurobi Afterwards Successful" << std::endl;
			current_best = solution;
		}
	}
}