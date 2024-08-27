#include "simulated_annealing.h"

SimulatedAnnealing::SimulatedAnnealing(const INITIAL_SOLUTION init) : init(init) {}

std::string SimulatedAnnealing::name() const {
	std::string id = "Simulated Annealing ";
	switch (init) {
	case PREPROCESS:
		id += "Preprocess";
		break;
	case RANDOM_RESTART:
		id += "Random Restart";
		break;
	case RANDOM:
		id += "Random";
		break;
	case GIVEN:
		id += "Given";
		break;
	default:
		throw std::runtime_error("Initial solution code is non-existent!");
		break;
	}
	return id;
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
	Validator FLV(instance);
	FLV.set_solution(SV.sol);
	if (!FLV.feasible()) {
		// Infeasible
		return;
	}
	switch (init) {
	case PREPROCESS:
	{
		solution = SV.sol;
	}
	break;
	case RANDOM_RESTART:
	{
		LocalSearch ls = LocalSearch(LocalSearch::RANDOM_RESTART);
		solution = ls.produce_initial_solution(instance, FLV, timer, report);
	}
	break;
	case RANDOM:
	{
		LocalSearch ls = LocalSearch(LocalSearch::RANDOM);
		solution = ls.produce_initial_solution(instance, FLV, timer, report);
	}
	break;
	case GIVEN:
	{
		solution = current_best.sol;
	}
	break;
	}

	if (!timer.in_time()) { return; }

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

	// for the cooling schedule
	const std::function<double(double)> cooling_schedule([](const double temperature) -> double { return 0.99 * temperature; });

	// Solving process
	double current_temperature = temperature_max;
	int max_hamming_distance = 2;
	double alpha = 0.7;
	int inner_iterations = int(instance.facilities) + int(0.5 * (instance.facilities * instance.facilities - instance.facilities));
	inner_iterations = int(alpha * inner_iterations);
	facility_vector neighbor;
	FLV.set_solution(solution);
	double current_solution_value = FLV.value();
	do {
		int iter = 0;
		while (iter++ < inner_iterations) {
			// Generate random neighbor, check if it's feasible
			neighbor = get_random_neighbor(max_hamming_distance, SV.sol, solution);
			FLV.set_solution(neighbor);
			if (!FLV.feasible()) { continue; }
			// Acceptance
			if (FLV.value() < current_solution_value) {
				solution = neighbor;
				current_solution_value = FLV.value();
				auto code = improve_solution(instance, current_best, solution, timer, report);
				if (code == Algorithm::TIMEOUT) {
					return;
				}
			}
			else {
				bool accept = unit_interval_random() < exp((current_solution_value - FLV.value())/current_temperature); //Enumerator is non-positive => exp in (0, 1] 
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

facility_vector get_random_neighbor(int max_hamming_distance, const facility_vector& not_unnecessary, facility_vector& solution) {
	assert(max_hamming_distance > 0);
	facility_vector neighbor = solution;
	std::vector<int> candidates(0);
	range_vector facility_range = range(not_unnecessary.size());
	asa::for_each(facility_range, [&candidates, &not_unnecessary](const int facility_id) {
		if (not_unnecessary[facility_id]) {
			candidates.push_back(facility_id);
		}
	});
	if (candidates.size() == 0) { return neighbor; }

	max_hamming_distance = (max_hamming_distance > candidates.size()) ? int(candidates.size()) : max_hamming_distance;

	// Choose a random hamming distance
	int hamming_distance = int(uniform(1, max_hamming_distance, true));
	while (hamming_distance-- > 0) {
		int tmp = int(uniform(0, double(candidates.size() - 1), true));
		int candidate = candidates[tmp];
		neighbor[candidate] = bool(1 - neighbor[candidate]);
		candidates.erase(std::begin(candidates) + tmp);
	}
	return neighbor;
}