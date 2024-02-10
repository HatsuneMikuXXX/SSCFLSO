#ifndef SSCFLSO_H
#define SSCFLSO_H
#include "../common.h"
#include "../typedefinitions.h"
struct SSCFLSO {
	int facilities = 0; // Number of facilities. Identifiers zero-based.
	int clients = 0; // Number of clients. Identifiers zero-based.
	demand_vector demands = std::vector<double>(); // Size |I|.
	capacity_vector capacities = std::vector<double>(); // Size |J|.
	facility_cost_vector facility_costs = std::vector<double>(); // Size |J|. Cost to open a facility.
	distribution_cost_matrix distribution_costs = std::vector<std::vector<double>>(); // Size |J| times |I|. Cost to distribute/of the distance from a facility to a client.
	preference_matrix preferences = std::vector<std::vector<int>>(); // Size |I| times |J|. First element (facility) is the most preferred.
};
#endif