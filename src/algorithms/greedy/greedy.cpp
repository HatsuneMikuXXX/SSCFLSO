#include "greedy.h"

std::string Greedy::name() const {
	return "Greedy";
}

bool Greedy::post_applyable() const {
	return false;
}

void Greedy::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	// preprocess
	Preprocess p = Preprocess();
	solution_and_value SV = { facility_vector(instance.facilities, 0), -1};
	p.solve(instance, SV, timer, report, false);
	if (SV.val == -1) { return; }

	// Compute utilities
	std::vector<std::pair<int, double>> utilities(instance.facilities);
	int index = 0;
	asa::generate(utilities, [&instance, &SV, &index]() -> std::pair<int, double> {
		if (SV.sol[index] == 0) {
			return std::pair<int, double>(index++, 0);
		}
		else {
			double total_cost = (instance.facility_costs[index] + asa::sum(instance.distribution_costs[index], 0));
			double utility = (total_cost == 0) ? DBL_MAX : instance.capacities[index] / total_cost;
			return std::pair<int, double>(index++, utility);
		}
	});
	asa::sort(utilities, [](const std::pair<int, double>& a, const std::pair<int, double>& b) -> bool { return a.second > b.second; }); //Sort in Desc

	// Greedy
	Validator FLV = Validator(instance);
	facility_vector solution = facility_vector(instance.facilities, 0);
	FLV.set_solution(solution);
	int index = 0;
	while (!FLV.feasible()) {
		int facility_id = utilities[index++].first;
		solution[facility_id] = 1;
		FLV.set_solution(solution);
	}
	improve_solution(instance, current_best, solution, timer, report);
	// Drop empty facilities afterwards
	FLV.drop_empty_facilities();
	improve_solution(instance, current_best, solution, timer, report);
	if (gurobi_afterwards && timer.in_time()) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}