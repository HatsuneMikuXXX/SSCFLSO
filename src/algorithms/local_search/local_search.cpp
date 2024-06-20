#include "local_search.h"


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

facility_vector LocalSearch::produce_initial_solution(const SSCFLSO& instance, Validator& FLV, Timer& timer, ReportResult& report) {
	facility_vector initial_solution(instance.facilities, 0);

	switch (this->init) {
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
			Preprocess p = Preprocess();
			solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
			p.solve(instance, SV, timer, report, false);

			bool stuck_in_local_optima = false;
			double rating = 0;
			facility_vector best_neighbor(instance.facilities);

			do {
				// Initial
				int facility_id = 0;
				asa::generate(initial_solution, [&facility_id, &SV]() -> int { return (SV.sol[facility_id++] == 0) ? 0 : flip(); });
				FLV.set_solution(initial_solution);
				rating = FLV.evaluate_inf_solution();
				// Search
				while (!FLV.feasible() && !stuck_in_local_optima && timer.in_time()) {
					stuck_in_local_optima = true;
					// Add-Remove-Neighborhood

					// Swap-Neighborhood // Quadratic amount many

					initial_solution = best_neighbor;
				}
			} while (!FLV.feasible() && timer.in_time());
		}
		break;
	default:
		break;
	}
	return initial_solution;
}

void LocalSearch::get_next_neighbor(Validator& FLV, facility_vector& solution, NEXT_NEIGHBOR nn_code) {
	assert(solution.size() > 0);
	//ASR
	std::vector<int> facility_range = range(solution.size());
	FLV.set_solution
	switch (nn_code) {
	case FIRST:

		break;
	case BEST:
		break;
	}
}


void LocalSearch::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	auto start = start_timer();
	Validator FLV = Validator(instance);
	facility_vector no_unnecessary_facilities = preprocess(instance);
	facility_vector solution = no_unnecessary_facilities;
	std::vector<facility_vector> neighborhood;
	std::vector<facility_vector> tmp; // Collect subneighborhoods and add them to neighborhood
	bool stuck_in_local_optima = false;
	while (!stuck_in_local_optima) {
		stuck_in_local_optima = true;
		// Collect neighborhoods
		neighborhood = {};
		tmp = swap_neighborhood(solution, no_unnecessary_facilities);
		neighborhood.insert(neighborhood.begin(), tmp.begin(), tmp.end());
		tmp = remove_neighborhood(solution);
		neighborhood.insert(neighborhood.begin(), tmp.begin(), tmp.end());
		// Conduct search
		FLV.set_solution(solution);
		double current_value = FLV.value();
		for (auto neighbor = neighborhood.begin(); neighbor != neighborhood.end(); neighbor++) {
			FLV.set_solution(*neighbor);
			if (FLV.feasible() && FLV.value() < current_value) {
				solution = *neighbor;
				current_value = FLV.value();
				stuck_in_local_optima = false;
			}
		}
		if (!within_time_limit(start, time_limit)) {
			return;
		}
		else {
			current_best = solution;
		}
	}
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
	return;
}
