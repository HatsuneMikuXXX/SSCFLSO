#ifndef SSCFLSOGUROBI_H
#define SSCFLSOGUROBI_H
#include <gurobi_c++.h>
#include "../common.h"
#include "../helpers/data_helper.h"
#include "../SSCFLSO/SSCFLSO.h"
typedef struct {
	std::vector<double> rFacility_vector = {};
	std::vector<std::vector<double>> rFacilityClient_vector = {};
} relaxed_solution;
relaxed_solution solve(const SSCFLSO& instance);
#endif