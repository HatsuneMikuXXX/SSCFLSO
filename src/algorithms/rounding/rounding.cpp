#include "rounding.h"

std::string Rounding::name() const {
	return "Rounding";
};

bool Rounding::post_applyable() const {
	return false;
}

void Rounding::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	Preprocess p = Preprocess();
	solution_and_value SV = { facility_vector(instance.facilities, 0), -1 };
	p.solve(instance, SV, timer, report, false);
	if (SV.val == -1) { return; }
	std::vector<double> relaxed_solution = solve_with_gurobi_afterwards(instance, current_best, SV.sol, timer, report, true); // Should be fast because continuous
	facility_vector solution(instance.facilities);
	// Round
	int index = 0;
	asa::generate(solution, [&relaxed_solution, &index]() -> bool { return relaxed_solution[index++] > 0; });
	// Close empty facilities
	Validator FLV(instance);
	FLV.set_solution(solution);
	FLV.drop_empty_facilities();
	improve_solution(instance, current_best, solution, timer, report);
	if (gurobi_afterwards && timer.in_time()) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}
