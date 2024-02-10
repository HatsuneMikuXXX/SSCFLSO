#ifndef SSCFLSOGUROBI_H
#define SSCFLSOGUROBI_H
#include <gurobi_c++.h>
#include "../common.h"
#include "../helpers/data_helper.h"
#include "../SSCFLSO/SSCFLSO.h"
std::vector<float> solve(const SSCFLSO& instance);
#endif