#ifndef SEMI_LAGRANGIAN_RELAXATION
#define SEMI_LAGRANGIAN_RELAXATION
#include "../algorithmClass.h"
#include "../stingy/stingy.h"
#include <gurobi_c++.h>
typedef struct {
	double LR_value;
	std::vector<double> subgradient_demand_constraint = {};
	std::vector<double> subgradient_capacity_constraint = {};
	std::vector<std::vector<double>> subgradient_preference_constraint = {};
} LR_value_and_subgradient;

typedef struct {
	facility_vector solution = {};
	std::vector<double> subgradient_demand_constraint = {};
} SLR_solution_and_subgradient;

typedef std::vector<double> semilagrange_multipliers;

class SemiLagrangianRelaxation : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit);
	std::string meta_information();
private:
	lagrangian_multipliers SGM(const SSCFLSO& instance, double UB);
	LR_value_and_subgradient LR(const SSCFLSO& instance, const lagrangian_multipliers& multipliers);
	SLR_solution_and_subgradient DAM(
		const SSCFLSO& instance,
		const lagrangian_multipliers& initial,
		const std::chrono::system_clock::time_point& start,
		const std::chrono::milliseconds& time_limit,
		facility_vector& current_best);
	SLR_solution_and_subgradient SLR(const SSCFLSO& instance, const semilagrange_multipliers& multipliers);
};
#endif