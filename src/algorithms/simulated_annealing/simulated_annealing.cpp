#include "simulated_annealing.h"


std::string SimulatedAnnealing::name() const {
	return "Simulated Annealing";
}

bool SimulatedAnnealing::post_applyable() const {
	return true;
}

void SimulatedAnnealing::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	facility_vector solution(instance.facilities, 0);
	
	// Preprocess
	Preprocess p = Preprocess();
	solution_and_value SV{facility_vector(instance.facilities, 0), -1};
	p.solve(instance, SV, timer, report, false);
	solution = SV.sol;
	if (SV.val == -1) {
		// Infeasible
		return;
	}

	// Parameters
	double temperature_min = 0.01;
	double temperature_max;
	{
		// Compute max temperature as max cost difference
		temperature_max = asa::sum(instance.facility_costs, -(*std::min(std::begin(instance.facility_costs), std::end(instance.facility_costs))));
		range_vector client_range = range(instance.clients);
		range_vector facility_range = range(instance.facilities);
		asa::for_each(client_range, [&instance, &temperature_max, &facility_range](const int client_id) {
			double min = DBL_MAX;
			double max = -1;
			asa::for_each(facility_range, [&instance, &min, &max, &client_id](const int facility_id) {
				double tmp = (instance.distribution_costs[facility_id][client_id]);
				min = tmp < min ? tmp : min;
				max = tmp > max ? tmp : max;
			});
			temperature_max += max - min;
		});
	}

	double inner_iterations = 30;
	const std::function<double(double)> cooling_schedule([](const double temperature) -> double { return 0.9 * temperature; });

	// Solving process
	double current_temperature = temperature_max;
	int max_hamming_distance = 2;
	facility_vector neighbor;
	Validator FLV(instance);
	FLV.set_solution(solution);
	double current_solution_value = FLV.value();
	do {
		int iter = 0;
		while (iter++ < inner_iterations) {
			// Generate random neighbor
			neighbor = get_random_neighbor(max_hamming_distance, SV.sol, solution);
			FLV.set_solution(neighbor);
			// Acceptance
			if (FLV.value() < current_solution_value) {
				solution = neighbor;
				current_solution_value = FLV.value();
				improve_solution(instance, current_best, solution, timer, report);
			}
			else {
				bool accept = random() < exp((current_solution_value - FLV.value())/current_temperature);
				if (accept) {
					solution = neighbor;
					current_solution_value = FLV.value();
				}
			}
		}
		// Cooling
		current_temperature = cooling_schedule(current_temperature);
	} while (current_temperature > temperature_min && timer.in_time());
	if (gurobi_afterwards && timer.in_time()) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}

facility_vector get_random_neighbor(int max_hamming_distance, const facility_vector& unnecessary, facility_vector& solution) {
	facility_vector neighbor = solution;
	std::vector<int> candidates(0);
	range_vector facility_range = range(unnecessary.size());
	asa::for_each(facility_range, [&candidates, &unnecessary](const int facility_id) {
		if (!unnecessary[facility_id]) {
			candidates.push_back(facility_id);
		}
	});
	max_hamming_distance = (max_hamming_distance > candidates.size()) ? candidates.size() : max_hamming_distance;
	// Choose a random hamming distance
	int hamming_distance = uniform(1, max_hamming_distance, true);
	while (hamming_distance-- > 0) {
		int tmp = uniform(0, candidates.size() - 1, true);
		int candidate = candidates[tmp];
		neighbor[candidate] = bool(1 - neighbor[candidate]);
		candidates.erase(std::begin(candidates) + tmp);
	}
	return neighbor;
}