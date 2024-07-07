#include "local_search.h"

LocalSearch::LocalSearch(){}

LocalSearch::LocalSearch(INITIAL_SOLUTION init) : init(init) {}

LocalSearch::LocalSearch(INITIAL_SOLUTION init, NEXT_NEIGHBOR next) : init(init), next(next) {}

std::string LocalSearch::name() const {
	std::string id = "Local Search: ";
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
	id += " + ";
	switch (next) {
	case BEST:
		id += "Best";
		break;
	case FIRST:
		id += "First";
		break;
	default:
		throw std::runtime_error("Next neighbor solution code is non-existent!");
		break;
	}
	return id;
}

bool LocalSearch::post_applyable() const {
	return true;
}

void LocalSearch::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	Validator FLV(instance);
	facility_vector solution = (init == GIVEN) ? current_best.sol : produce_initial_solution(instance, FLV, timer, report);	

	switch (init) {
	case GIVEN:
		FLV.set_solution(solution);
		if (!FLV.feasible()) {
			if (!attempt_to_find_feasible_solution(solution, FLV)) {
				return;
			}
		}
		while (get_next_neighbor(FLV, solution)) {
			if (improve_solution(instance, current_best, solution, timer, report) == TIMEOUT) {
				break;
			}
		}
		break;
	case PREPROCESS:
		if (no_facility_is_open(solution)) {
			return;
		}
		while (get_next_neighbor(FLV, solution)) {
			if (improve_solution(instance, current_best, solution, timer, report) == TIMEOUT) {
				break;
			}
		}
		break;
	case RANDOM:
	case RANDOM_RESTART:
		improve_solution(instance, current_best, solution, timer, report);
		{
			std::vector<SolutionContainer> SC_collection = { SolutionContainer(solution) };
			int sc_index = 0;

			while (timer.in_time()) {
				while (get_next_neighbor(FLV, solution)) {
					improve_solution(instance, current_best, solution, timer, report);
					SC_collection[sc_index].add(solution);
				}
				// We are stuck in a local optima, repeat the search from a freshly new solution
				do {
					solution = produce_initial_solution(instance, FLV, timer, report);
				} while (asa::any_of(SC_collection, [&solution](const SolutionContainer& sc) -> bool { return sc.contains(solution); }));

				SC_collection.push_back(SolutionContainer(solution));
				sc_index++;
			}
		}
		break;
	default:
		break;
	}
	if (gurobi_afterwards && timer.in_time()) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}

bool attempt_to_find_feasible_solution(facility_vector& initial_solution, Validator& FLV, int maxIter = 10e5) {
	range_vector facility_range = range(initial_solution.size());
	facility_vector tmp_neighbor;
	facility_vector best_neighbor;
	// Search
	FLV.set_solution(initial_solution);
	double rating = FLV.evaluate_inf_solution();
	bool stuck_in_local_optima = false;
	int iter = 0;
	while (!FLV.feasible() && !stuck_in_local_optima && iter++ < maxIter) {
		stuck_in_local_optima = true;
		// Add-Remove-Neighborhood
		asa::for_each(facility_range, [&initial_solution, &rating, &best_neighbor, &tmp_neighbor, &FLV, &stuck_in_local_optima](const int facility_id) {
			tmp_neighbor = initial_solution;
			tmp_neighbor[facility_id] = 1 - tmp_neighbor[facility_id];
			FLV.set_solution(tmp_neighbor);
			if (FLV.evaluate_inf_solution() < rating) {
				best_neighbor = tmp_neighbor;
				rating = FLV.evaluate_inf_solution();
				stuck_in_local_optima = false;
			}
		});
		initial_solution = best_neighbor;
		FLV.set_solution(initial_solution);
	}
	return FLV.feasible();
}

facility_vector LocalSearch::produce_initial_solution(const SSCFLSO& instance, Validator& FLV, Timer& timer, ReportResult& report) const {
	facility_vector initial_solution(instance.facilities, 0);

	switch (init) {
	case PREPROCESS:
		{
			Preprocess p = Preprocess();
			solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
			p.solve(instance, SV, timer, report, false);
			initial_solution = SV.sol;
		}
		break;
	case RANDOM_RESTART:
		{
			Preprocess p = Preprocess();
			solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
			p.solve(instance, SV, timer, report, false);
			do {
				initial_solution.clear();
				initial_solution.resize(instance.facilities);
				int facility_id = 0;
				asa::generate(initial_solution, [&facility_id, &SV]() -> int { return (SV.sol[facility_id++] == 0) ? 0 : flip(); });
				FLV.set_solution(initial_solution);
			} while (!FLV.feasible() && timer.in_time());
		}
		break;
	case RANDOM:
		{
			// Preparation
			Preprocess p = Preprocess();
			solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
			p.solve(instance, SV, timer, report, false);

			std::vector<SolutionContainer> SC_collection(0);
			int sc_index = -1;

			bool stuck_in_local_optima = false;
			double rating = 0;
			facility_vector best_neighbor(instance.facilities);
			facility_vector tmp_neighbor(instance.facilities);
			range_vector facility_range = range(instance.facilities);

			do {
				// Initial
				initial_solution.clear();
				initial_solution.resize(instance.facilities);
				int facility_id = 0;
				asa::generate(initial_solution, [&facility_id, &SV]() -> int { return (SV.sol[facility_id++] == 0) ? 0 : flip(); });
				
				// If solution was already seen, do it over
				bool solution_already_checked = asa::any_of(SC_collection, [&initial_solution](const SolutionContainer& sc) -> bool { return sc.contains(initial_solution); });
				if (solution_already_checked) { continue; }
				SC_collection.push_back(SolutionContainer(initial_solution));
				sc_index++;

				// Search
				FLV.set_solution(initial_solution);
				rating = FLV.evaluate_inf_solution();
				while (!FLV.feasible() && !stuck_in_local_optima && timer.in_time()) {
					stuck_in_local_optima = true;
					// Add-Remove-Neighborhood
					asa::for_each(facility_range, [&initial_solution, &rating, &best_neighbor, &tmp_neighbor, &FLV, &stuck_in_local_optima](const int facility_id) {
						tmp_neighbor = initial_solution;
						tmp_neighbor[facility_id] = 1 - tmp_neighbor[facility_id];
						FLV.set_solution(tmp_neighbor);
						if (FLV.evaluate_inf_solution() < rating) {
							best_neighbor = tmp_neighbor;
							rating = FLV.evaluate_inf_solution();
							stuck_in_local_optima = false;
						}
					});
					initial_solution = best_neighbor;
					FLV.set_solution(initial_solution);
					SC_collection[sc_index].add(initial_solution);
				}
			} while (!FLV.feasible() && timer.in_time());
		}
		break;
	default:
		break;
	}
	return initial_solution;
}

bool LocalSearch::get_next_neighbor(Validator& FLV, facility_vector& solution) const {
	assert(solution.size() > 0);
	FLV.set_solution(solution);
	double solution_value = FLV.value();
	range_vector facility_range = range(solution.size());
	facility_vector best_neighbor = solution;
	//ASR
	{
		//Add-Neighborhood
		facility_vector tmp = solution;
		auto it = std::begin(facility_range);
		while (it != std::end(facility_range)) {
			if (solution[*it]) { it++; continue; }
			tmp[*it] = 1;
			FLV.set_solution(tmp);
			if (FLV.feasible() && FLV.value() < solution_value) {
				switch (next) {
				case FIRST:
					solution = tmp;
					return true;
				case BEST:
					best_neighbor = tmp;
					solution_value = FLV.value();
					break;
				}
			}
			tmp[*it] = 0;
			it++;
		}
		// Swap-Neighborhood
		auto it1 = std::begin(facility_range);
		while (it1 != std::end(facility_range)) {
			if (solution[*it1]) { it1++; continue; }
			tmp[*it1] = 1;
			auto it2 = std::begin(facility_range);
			while (it2 != std::end(facility_range)) {
				if (!solution[*it2]) { it2++; continue; }
				tmp[*it2] = 0;
				FLV.set_solution(tmp);
				if (FLV.feasible() && FLV.value() < solution_value) {
					switch (next) {
					case FIRST:
						solution = tmp;
						return true;
					case BEST:
						best_neighbor = tmp;
						solution_value = FLV.value();
						break;
					}
				}
				tmp[*it2] = 1;
				it2++;
			}
			tmp[*it1] = 0;
			it1++;
		}

		// Remove-Neighborhood
		auto it = std::begin(facility_range);
		while (it != std::end(facility_range)) {
			if (!solution[*it]) { it++; continue; }
			tmp[*it] = 0;
			FLV.set_solution(tmp);
			if (FLV.feasible() && FLV.value() < solution_value) {
				switch (next) {
				case FIRST:
					solution = tmp;
					return true;
				case BEST:
					best_neighbor = tmp;
					solution_value = FLV.value();
					break;
				}
			}
			tmp[*it] = 1;
			it++;
		}
	}
	return (solution == best_neighbor) ? false : (solution = best_neighbor) == solution; //Last expression is equivalent to true
}