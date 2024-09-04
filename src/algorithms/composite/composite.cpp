#include "composite.h"

Composite::Composite(const std::vector<Algorithm*>& algorithms) : algorithms(algorithms) {}

std::string Composite::name() const {
	std::string res = "Composition of: ";
	bool check_for_applicability = false;
	asa::for_each(algorithms, [&res, &check_for_applicability](const Algorithm* algo_ptr) {
		if (algo_ptr == NULL || (check_for_applicability && !algo_ptr->post_applyable())) {
			return;
		}
		check_for_applicability = true;
		res += algo_ptr->name() + " ";
	});
	return res;
}

bool Composite::post_applyable() const {
	return false;
}

void Composite::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	bool check_for_applicability = false;
	asa::for_each(algorithms, [&check_for_applicability, &instance, &current_best, &timer, &report](const Algorithm* algo_ptr) {
		if (algo_ptr != NULL && (!check_for_applicability || algo_ptr->post_applyable())) {
			check_for_applicability = true;
			algo_ptr->solve(instance, current_best, timer, report, false);
		}
	});
	if (gurobi_afterwards) { solve_with_gurobi_afterwards(instance, current_best, current_best.sol, timer, report); }
}
