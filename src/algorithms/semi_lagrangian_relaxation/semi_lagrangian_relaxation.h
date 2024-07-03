#ifndef SEMI_LAGRANGIAN_RELAXATION
#define SEMI_LAGRANGIAN_RELAXATION
#include "../../algorithms/algorithmClass.h"
#include <gurobi_c++.h>
#include "../Preprocess/preprocess.h"

class SemiLagrangianRelaxation : public Algorithm {
public:
	SemiLagrangianRelaxation(bool homogenous_weight_update);
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
private:
	bool homogenous_weight_update{false};
	std::vector<std::vector<double>> weight_update_matrix(const SSCFLSO& instance) const;
	GRBModel constructSLRModel(const SSCFLSO& instance) const;
};
#endif