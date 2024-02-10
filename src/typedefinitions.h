#ifndef TYPE_DEFINITIONS_H
#define TYPE_DEFINITIONS_H
typedef std::vector<int> facility_vector;
typedef std::vector<int> client_vector;
typedef std::vector<double> demand_vector;
typedef std::vector<double> capacity_vector;
typedef std::vector<double> facility_cost_vector;
typedef std::vector<std::vector<double>> distribution_cost_matrix;
typedef std::vector<std::vector<int>> preference_matrix;
typedef std::vector<int> client_facility_assignment;
typedef std::function<bool(int)> facility_predicate;
#endif 