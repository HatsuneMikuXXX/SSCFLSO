#include "grasp.h"

GRASP::GRASP() {}

GRASP::GRASP(double RCL_percentile) : RCL_percentile(RCL_percentile) {};

GRASP::GRASP(int maxIter, double RCL_percentile) : maxIter(maxIter), RCL_percentile(RCL_percentile) {}

std::string GRASP::name() const {
	return "GRASP RCL percentile " + std::to_string(RCL_percentile);
};

bool GRASP::post_applyable() const {
	return false;
}

void GRASP::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	Validator FLV(instance);
	facility_vector solution(instance.facilities, 0);
	int iter = 0;
	// Prepare 
	Preprocess p = Preprocess();
	solution_and_value SV = {facility_vector(instance.facilities, 0), -1};
	p.solve(instance, SV, timer, report, false);
	FLV.set_solution(SV.sol);
	if (!FLV.feasible()) { return; }
	LocalSearch ls = LocalSearch(LocalSearch::GIVEN, LocalSearch::FIRST);
	std::vector<SolutionContainer> sc_collection;
	int sc_index = 0;
	while (iter++ < maxIter && timer.in_time()) {
		greedy_random(FLV, SV.sol, solution);
		if (asa::any_of(sc_collection, [&solution](const SolutionContainer& sc) -> bool { return sc.contains(solution); })) {
			continue;
		}
		
		sc_collection.push_back(SolutionContainer(solution));
		improve_solution(instance, current_best, solution, timer, report);
		FLV.set_solution(solution);
		// Perform "Local Search First"
		while (ls.get_next_neighbor(FLV, solution)) {
			sc_collection[sc_index].add(solution);
			auto code = improve_solution(instance, current_best, solution, timer, report);
			if (code == Algorithm::TIMEOUT) {
				return;
			}
		}
		sc_index++;
	}
	if (gurobi_afterwards && timer.in_time()) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}

void GRASP::greedy_random(Validator& FLV, const facility_vector& CL, facility_vector& solution) const {
	range_vector facility_range = range(solution.size());
	
	facility_vector tmp(solution.size(), 0);
	FLV.set_solution(tmp);
	do {
		// Compute utilities
		std::vector<std::pair<int, double>> utilities({});

		asa::for_each(facility_range, [&tmp, &FLV, &CL, &utilities](const int facility_id) {
			if (CL[facility_id] == 0 || tmp[facility_id] == 1) {
				return;
			}
			else {
				tmp[facility_id] = 1;
				FLV.set_solution(tmp);
				tmp[facility_id] = 0;
				utilities.push_back(std::pair<int, double>(facility_id, -FLV.value())); // Low cost = High utility
			}
		});
		asa::sort(utilities, [](const std::pair<int, double>& a, const std::pair<int, double>& b) -> bool { return a.second > b.second; }); //Sort in Desc
		// Construct RCL
		double threshold = (1. - RCL_percentile) * utilities[0].second + RCL_percentile * utilities.back().second;
		auto it = std::find_if(std::begin(utilities), std::end(utilities), [&threshold](const std::pair<int, double>& elem) -> bool { return elem.second < threshold - 10e-7; });
		utilities.erase(it, utilities.end());
		
		// Choose element at random
		int index = int(uniform(0, double(utilities.size()) - 1, true));
		int facility_id = utilities[index].first;
		tmp[facility_id] = 1;
		FLV.set_solution(tmp);
	} while (!FLV.feasible());
	solution = tmp;
	return;
}