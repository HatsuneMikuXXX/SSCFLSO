#ifndef LAGRANGIAN_RELAXATION
#define LAGRANGIAN_RELAXATION
#include "../../algorithms/algorithmClass.h"
#include <gurobi_c++.h>
#include "../local_search/local_search.h"

class LagrangianRelaxation : public Algorithm {
public:
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;

	struct Weights {
		std::vector<double> demand_weights{ std::vector<double>(0)};
		std::vector<double> capacity_weights{ std::vector<double>(0) };
		std::vector<std::vector<double>> preference_weights{ std::vector<double>(0) };
	};

private:
	double get_gradient_magnitude(const range_vector& facility_range, const range_vector& client_range, const SSCFLSO& instance, GRBModel& model) const;
	void update_weights(const range_vector& facility_range, const range_vector& client_range, Weights& weights, const SSCFLSO& instance, double beta, double UB, GRBModel& model) const;
	void update_weights_of_model(const SSCFLSO& instance, const range_vector& facility_range, const range_vector& client_range, GRBModel& model, const Weights& weights) const;
	GRBModel constructLRModel(const SSCFLSO& instance) const;
};
#endif