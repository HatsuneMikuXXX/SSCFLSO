#ifndef TYPE_DEFINITIONS_H
#define TYPE_DEFINITIONS_H
#include "common.h"
typedef std::vector<int> facility_vector;
typedef std::vector<int> client_vector;
typedef std::vector<double> demand_vector;
typedef std::vector<double> capacity_vector;
typedef std::vector<double> facility_cost_vector;

typedef std::vector<double> facility_distribution_cost_vector;
typedef std::vector<facility_distribution_cost_vector> distribution_cost_matrix;

typedef std::vector<int> client_preference_vector;
typedef std::vector<client_preference_vector> preference_matrix;

typedef std::vector<int> client_facility_assignment;
typedef std::vector<std::vector<int>> assignment_matrix;

typedef std::function<bool(int)> facility_predicate;

typedef struct solution_and_value {
	facility_vector sol;
	double val;
};
#endif 