#ifndef SEMI_LAGRANGIAN_RELAXATION
#define SEMI_LAGRANGIAN_RELAXATION
#include "../algorithm.h"
#include <gurobi_c++.h>
typedef struct {
	std::vector<double> RC_exact_demand = {};
	std::vector<double> RC_capacity = {};
	std::vector<std::vector<double>> RC_preferences = {};
} lagrange_multipliers;

typedef std::vector<double> semilagrange_multipliers;

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

facility_vector solve(const SSCFLSO& instance);
LR_value_and_subgradient LR(const SSCFLSO& instance, const lagrange_multipliers& multipliers);

lagrange_multipliers SGM(const SSCFLSO& instance, double UB);
facility_vector UBH(const SSCFLSO& instance);
SLR_solution_and_subgradient DAM(const SSCFLSO& instance, const lagrange_multipliers& initial);
SLR_solution_and_subgradient solve_SLR(const SSCFLSO& instance, const semilagrange_multipliers& multipliers);



#endif