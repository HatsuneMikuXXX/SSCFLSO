#include "preprocess.h"

std::string Preprocess::name() {
	return "Preprocessing";
}

void Preprocess::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) {
	facility_vector solution = facility_vector(instance.facilities, 1);
	{
		// Remove facilities with a too small capacity
		const double minimum_demand = *min_element(std::begin(instance.demands), std::end(instance.demands));
		facility_predicate facilities_to_keep = [&instance, &minimum_demand](const int& facility) 
			-> bool { return instance.capacities[facility] >= minimum_demand; };
		filter(solution, facilities_to_keep);
	}
	{
		// Remove unnecessary facilities (too popular-issue)
		Validator FLV = Validator(instance);
		FLV.set_solution(solution);
		{
			facility_vector exceeds_capacity;
			while (!FLV.feasible() && std::any_of(std::begin(solution), std::end(solution), [](const int& facility) -> bool {return facility == 1; })) {
				// Subtract facilities that exceed capacity
				exceeds_capacity = FLV.exceeds_capacity();
				std::transform(solution.begin(), solution.end(), exceeds_capacity.begin(), solution.begin(), std::minus<int>());
				FLV.set_solution(solution);
			}
		}
	}
	{
		// Remove unnecessary facilities (too unpopular-issue)
		// First, compute some helpful information/data structures
		const double total_demand = sum(instance.demands);
		std::vector<int> range(instance.facilities);
		std::iota(std::begin(range), std::end(range), 0);
		const std::vector<int> range = range;

		// Define heuristic
		const std::function<int(const facility_vector&)> LB_facilities_required_heuristic = [&instance, &total_demand, &range](const facility_vector& solution) {
			// Sort capacities
			capacity_vector capacities = {};
			std::for_each(std::begin(range), std::end(range), 
				[&instance, &solution, &capacities](const int& facility_id) { if (solution[facility_id] == 1) {
				capacities.push_back(instance.capacities[facility_id]);
			}}); // Consider only capacities of open facilities
			std::sort(std::begin(capacities), std::end(capacities), [](const double& a, const double& b) -> bool {return a > b; }); // Sort DESC
			
			// Compute minimum number of facilities required
			int min = 0;
			double minCapacity = 0;
			std::for_each(std::begin(capacities), std::end(capacities), [&min, &minCapacity, &total_demand](const double& c) {
				if (minCapacity < total_demand) {
					min++;
					minCapacity += c;
				}
			});
			return (minCapacity >= total_demand) ? min : -1;;
		};

		// Define function to rank facilities
		std::vector<std::map<int, int>> rankings;
		const std::function<void()> assign_ranks([&instance, &solution, &rankings]() {
			rankings.clear();
			std::vector<int> range(instance.clients);
			std::iota(std::begin(range), std::end(range), 0);
			std::for_each(std::begin(range), std::end(range), [&instance, &solution, &rankings](const int& client_id) {
				std::map<int, int> ranking = {};
				int rank = 0;
				const client_preference_vector& p = instance.preferences[client_id];
				std::for_each(std::begin(p), std::end(p), [&rank, &ranking, &solution](const int& facility_id) {
					if (solution[facility_id] == 1) {
						ranking.insert({ facility_id, rank });
						rank++;
					}
				});
				rankings.push_back(ranking);
			});
		});

		// Determine unnecessary facilities here
		bool cannot_remove_further_unnecessary_facilities = false;
		while (!cannot_remove_further_unnecessary_facilities) {
			int min = LB_facilities_required_heuristic(solution);
			if (min == -1) { break; }
			assign_ranks();
			
			// Compute unnecessary facilities - start by assuming that all facilities are unnecessary
			facility_vector unnecessary_facility_candidates = solution;
			int number_unnecessary_facilities = std::accumulate(std::begin(unnecessary_facility_candidates), std::end(unnecessary_facility_candidates), 0);
			int counter = 0;
			while (counter < instance.clients && number_unnecessary_facilities > 0) {
				int client = counter;
				std::for_each(std::begin(range), std::end(range), 
					[&instance, &min, &unnecessary_facility_candidates,	&number_unnecessary_facilities,	&rankings, &client](const int& facility_id) {
						if (unnecessary_facility_candidates[facility_id] == 1 && rankings[client][facility_id] <= instance.facilities - min) {
							unnecessary_facility_candidates[facility_id] = 0;
							number_unnecessary_facilities--;
						}
					});
				if (number_unnecessary_facilities == 0) {
					cannot_remove_further_unnecessary_facilities = true;
					break;
				}
				counter++;
			}			
			
			// Update
			std::for_each(std::begin(range), std::end(range), [&solution, &unnecessary_facility_candidates](const int& facility_id) {
				solution[facility_id] *= (1 - unnecessary_facility_candidates[facility_id]);
			});
		}
	}
	improve_solution(instance, current_best, solution, timer, report);
	if (gurobi_afterwards) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}
