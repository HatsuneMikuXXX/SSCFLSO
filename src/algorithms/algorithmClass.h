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

typedef struct eliminate_variables {
	facility_vector facilities_keep_closed = {};
	assignment_matrix distribution_keep_closed = {};
};

typedef struct initial_solution {
	facility_vector initial_facilities = {};
	assignment_matrix initial_distribution = {};
};

typedef struct lagrangian_multipliers {
	std::vector<double> demand_constraint_weight;
	std::vector<double> capacity_constraint_weight;
	std::vector<std::vector<double>> preference_constraint_weight;
} lagrangian_multipliers;

class Algorithm {
	public:
		virtual void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) = 0;
		virtual std::string meta_information() = 0;

		std::chrono::system_clock::time_point start_timer();
		std::chrono::milliseconds get_elapsed_time_ms(const std::chrono::system_clock::time_point& start);
		bool within_time_limit(const std::chrono::system_clock::time_point& start, const std::chrono::milliseconds& time_limit);

		facility_vector preprocess(const SSCFLSO& instance);
		int lower_bound_facilities_required(const SSCFLSO& instance, const facility_vector& facilities_to_consider);
		std::vector<facility_vector> add_neighborhood(const facility_vector& x, const facility_vector& preprocessed);
		std::vector<facility_vector> remove_neighborhood(const facility_vector& x);
		std::vector<facility_vector> swap_neighborhood(const facility_vector& x, const facility_vector& preprocessed);
		relaxed_solution solve_linear_relaxation(const SSCFLSO& instance, const eliminate_variables& variables, const initial_solution* init);
		extended_solution solve_lagrangian_relaxation(const SSCFLSO& instance, const lagrangian_multipliers& multipliers);
	protected:
		const std::string PATH = "src/algorithms/";
};
#endif