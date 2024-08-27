#include "gurobi.h"


std::string Gurobi::name() const {
	return "Gurobi";
}

bool Gurobi::post_applyable() const {
	return false;
}

void Gurobi::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	solve_with_gurobi_afterwards(instance, current_best, facility_vector(instance.facilities), timer, report);
}