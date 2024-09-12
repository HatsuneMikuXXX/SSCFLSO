#include "eager_search.h"

std::string EagerSearch::name() const {
	return "Eager Search";
}

bool EagerSearch::post_applyable() const {
	return false;
}

void EagerSearch::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	// Preparation
	Validator FLV(instance);
	
	// Avoid unnecessary facilities
	solution_and_value SV = {facility_vector(instance.facilities, 0), -1};
	Preprocess p = Preprocess();
	p.solve(instance, SV, timer, report, false);
	if (no_facility_is_open(SV.sol)) { return; }

	do{
		// Generate a random candidate
		facility_vector solution(instance.facilities);
		int facility_id = 0;
		asa::generate(solution, [&facility_id, &SV]() -> int { return (SV.sol[facility_id++] == 0) ? 0 : flip(); });
		
		// Attempt to find a feasible solution via LSF (AR neighborhood)
		if(!attempt_to_find_feasible_solution(solution, FLV, instance.facilities)) {
			continue;
		}
		improve_solution(instance, current_best, solution, timer, report);

		// Apply usual LSF
		LocalSearch ls = LocalSearch(LocalSearch::GIVEN, LocalSearch::FIRST);
		ls.solve(instance, current_best, timer, report, false);
	}while(true);
}
