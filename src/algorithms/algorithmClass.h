#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <gurobi_c++.h>
#include "../typedefinitions.h"
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../helpers/data_helper.h"

typedef struct relaxed_solution {
	std::vector<double> rFacility_vector = {};
	std::vector<std::vector<double>> rAssignment_vector = {};
} relaxed_solution;

typedef struct extended_solution {
	facility_vector ES_facilities;
	std::vector<std::vector<int>> ES_extended_assignment;
} extended_solution;

typedef struct lagrangian_multipliers {
	std::vector<double> demand_constraint_weight;
	std::vector<double> capacity_constraint_weight;
	std::vector<std::vector<double>> preference_constraint_weight;
} lagrangian_multipliers;

class Algorithm {
	public:
		virtual void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) = 0;
		virtual std::string meta_information() = 0;

		static std::chrono::system_clock::time_point start_timer();
		static std::chrono::milliseconds get_elapsed_time_ms(const std::chrono::system_clock::time_point& start);
		static bool within_time_limit(const std::chrono::system_clock::time_point& start, const std::chrono::milliseconds& time_limit);

		static facility_vector preprocess(const SSCFLSO& instance);
		static std::vector<facility_vector> add_neighborhood(const facility_vector& x, const facility_vector& preprocessed);
		static std::vector<facility_vector> remove_neighborhood(const facility_vector& x);
		static std::vector<facility_vector> swap_neighborhood(const facility_vector& x, const facility_vector& preprocessed);
		static relaxed_solution solve_linear_relaxation(const SSCFLSO& instance);
		static extended_solution solve_lagrangian_relaxation(const SSCFLSO& instance, const lagrangian_multipliers& multipliers);
	protected:
		static const std::string PATH;
};
#endif