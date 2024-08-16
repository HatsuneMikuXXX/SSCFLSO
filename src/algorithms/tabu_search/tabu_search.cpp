#include "tabu_search.h"

TabuSearch::TabuSearch(INITIAL_SOLUTION init) : init(init) {}

std::string TabuSearch::name() const {
	std::string id = "Tabu search ";
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

bool TabuSearch::post_applyable() const {
	return true;
}

void TabuSearch::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	// Prepare
	Validator FLV(instance);
	range_vector facility_range = range(instance.facilities);
	Preprocess p = Preprocess();
	solution_and_value SV{facility_vector(instance.facilities, 0), -1};
	p.solve(instance, SV, timer, report, false);
	FLV.set_solution(SV.sol);
	if (!FLV.feasible()) {
		// Infeasible
		return;
	}

	facility_vector solution(instance.facilities, 0);
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

	// Search
	const int tabu_size = 10;
	std::vector<facility_vector> tabu_list(tabu_size, facility_vector(instance.facilities, 0));
	tabu_list[0] = solution;
	int tabu_index = 1;
	std::function<bool(const facility_vector&)> is_tabu([&tabu_list](const facility_vector& solution) -> bool {
		return asa::any_of(tabu_list, [&solution](const facility_vector& tabu) -> bool {return tabu == solution; });
	});
	bool search_exhausted = false;
	facility_vector best_neighbor = solution;
	facility_vector current_neighbor = solution;
	do {
		search_exhausted = true;
		double best_value = DBL_MAX;
		// AR neighborhood 
		asa::for_each(facility_range, [&search_exhausted, &is_tabu, &best_value, &best_neighbor, &current_neighbor, &FLV, &solution, &SV](const int facility_id) {
			if (SV.sol[facility_id] == 0) { return; }
			current_neighbor[facility_id] = bool(1 - solution[facility_id]);
			if (is_tabu(current_neighbor)) {
				current_neighbor[facility_id] = bool(solution[facility_id]);
				return;
			}
			FLV.set_solution(current_neighbor);
			if (FLV.feasible() && FLV.value() < best_value) {
				search_exhausted = false;
				best_neighbor = current_neighbor;
				best_value = FLV.value();
			}
			current_neighbor[facility_id] = bool(solution[facility_id]);
		});
		solution = best_neighbor;
		tabu_list[tabu_index] = solution;
		tabu_index = (tabu_index == tabu_size - 1) ? 0 : tabu_index + 1;
		improve_solution(instance, current_best, solution, timer, report);
	} while (!search_exhausted && timer.in_time());
	if (gurobi_afterwards && timer.in_time()) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}
