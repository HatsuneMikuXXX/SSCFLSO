#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <gurobi_c++.h>
#include "../SSCFLSO_model/SSCFLSO_validator.h"
#include "../helpers/utility.h"
#include "../helpers/timer.h"
#include "../tracking/report_result.h"

class Algorithm {
public:
	virtual void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const = 0;
	virtual std::string name() const = 0;
	virtual bool post_applyable() const = 0;
	enum UPDATE_CODE {
		IMPROVED = 0,
		NOT_IMPROVED = 1,
		TIMEOUT = 2,
		TIMER_NOT_RUNNING = 3,
	};
	static UPDATE_CODE improve_solution(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& new_solution, Timer& timer, ReportResult& report);
	std::vector<double> solve_with_gurobi_afterwards(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& initial, Timer& timer, ReportResult& report, bool continuous = false) const;
};
#endif