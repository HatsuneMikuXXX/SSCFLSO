#ifndef SEMI_LAGRANGIAN_RELAXATION
#define SEMI_LAGRANGIAN_RELAXATION
#include "../../algorithms/algorithmClass.h"
#include <gurobi_c++.h>



class SemiLagrangianRelaxation : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards);
	std::string meta_information();
private:
	std::vector<std::vector<double>> weight_update_matrix(const SSCFLSO& instance);
	GRBModel constructSLRModel(const SSCFLSO& instance);
};
#endif