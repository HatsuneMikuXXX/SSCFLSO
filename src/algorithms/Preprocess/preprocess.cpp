#include "preprocess.h"

std::string Preprocess::name() {
	return "Preprocessing";
}

void Preprocess::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) {
	Validator FLV = Validator(instance);
	facility_vector solution = facility_vector(instance.facilities, 1);
	{
		// Remove facilities with a too small capacity
		double minimum_demand = *min_element(instance.demands.begin(), instance.demands.end());
		capacity_vector capacities = instance.capacities;
		facility_predicate predicate = [capacities, minimum_demand](int facility) { return capacities[facility] >= minimum_demand; };
		filter(solution, predicate);
	}
	{
		// Remove unnecessary facilities (too popular-issue)
		FLV.set_solution(solution);
		{
			facility_vector exceeds_capacity;
			while (!FLV.feasible() && !is_empty(solution)) {
				// Subtract facilities that exceed capacity
				exceeds_capacity = FLV.exceeds_capacity();
				std::transform(solution.begin(), solution.end(), exceeds_capacity.begin(), solution.begin(), std::minus<int>());
				FLV.set_solution(solution);
			}
		}
	}
	{
		// Remove unnecessary facilities (too unpopular-issue)
		const std::function<bool(const double&, const double&)> is_less = [](const double& a, const double& b) { return a < b; };
		const std::function<int(const facility_vector&)> LB_facilities_required_heuristic = [instance, is_less](const facility_vector& current_solution) {
			// Compute total demand
			double total_demand = sum(instance.demands);
			// Sort capacities
			capacity_vector capacities = {};
			for (int j = 0; j < instance.facilities; j++) {
				if (current_solution[j] == 0) { continue; }
				bisect_insert(capacities, instance.capacities[j], is_less, false);
			}
			// Compute minimum number of facilities required
			int min = 0;
			double minCapacity = 0;
			for (auto it = capacities.begin(); it != capacities.end(); it++) {
				if (minCapacity < total_demand) {
					min++;
					minCapacity += *it;
				}
				else { break; }
			}
			return (minCapacity >= total_demand) ? min : -1;;
		};

		bool cannot_remove_further_unnecessary_facilities = false;
		while (!cannot_remove_further_unnecessary_facilities) {
			int min = LB_facilities_required_heuristic(solution);
			if (min == -1) { break; }
			// Compute rankings (low rank value = high preference)
			std::vector<std::map<int, int>> rankings;
			{
				for (int i = 0; i < instance.clients; i++) {
					std::map<int, int> ranking = {};
					int rank = 0;
					for (auto it = instance.preferences[i].begin(); it != instance.preferences[i].end(); it++) {
						if (solution[*it] == 1) {
							ranking[*it] = rank;
							rank++;
						}
					}
					rankings.push_back(ranking);
				}
			}
			
			// Compute unnecessary facilities - start by assuming that all facilities are unnecessary
			facility_vector unnecessary_facility_candidates = solution;
			int number_unnecessary_facilities = sum(solution);
			for (int i = 0; i < instance.clients && number_unnecessary_facilities > 0; i++) {
				for (int j = 0; j < instance.facilities; j++) {
					// Identify facilities that are not unnecessary
					if (unnecessary_facility_candidates[j] == 0) { continue; }
					if (rankings[i][j] <= instance.facilities - min) {
						unnecessary_facility_candidates[j] = 0;
						number_unnecessary_facilities -= 1;
					}
				}
				if (number_unnecessary_facilities == 0) { 
					cannot_remove_further_unnecessary_facilities = true;
					break; 
				}
			}
			
			// Update
			for (int j = 0; j < instance.facilities; j++) {
				if (unnecessary_facility_candidates[j] == 1) {
					solution[j] = 0;
				}
			}
		}
	}
	improve_solution(instance, current_best, solution, timer, report);
	if (gurobi_afterwards) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}
