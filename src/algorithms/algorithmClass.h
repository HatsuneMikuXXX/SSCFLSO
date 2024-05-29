#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <gurobi_c++.h>
#include "../typedefinitions.h"
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../helpers/utility.h"
#include "../helpers/timer.h"
#include "../helpers/report_result.h"

class Algorithm {
public:
	virtual void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) = 0;
	virtual std::string name() = 0;
protected:
	enum UPDATE_CODE {
		IMPROVED = 0,
		NOT_IMPROVED = 1,
		TIMEOUT = 2,
	};
	UPDATE_CODE improve_solution(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& new_solution, Timer& timer, ReportResult& report, bool searchingForFeasibleSolution = false);
	void solve_with_gurobi_afterwards(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& initial, Timer& timer, ReportResult& report);
};
#endif