#ifndef SSCFLSO_H
#define SSCFLSO_H
#include "../common.h"
#include "../typedefinitions.h"
struct SSCFLSO {
	int facilities = 0; // Number of facilities. Identifiers zero-based.
	int clients = 0; // Number of clients. Identifiers zero-based.
	demand_vector demands = demand_vector(); // Size |I|.
	capacity_vector capacities = capacity_vector(); // Size |J|.
	facility_cost_vector facility_costs = facility_cost_vector(); // Size |J|. Cost to open a facility.
	distribution_cost_matrix distribution_costs = distribution_cost_matrix(); // Size |J| times |I|. Cost to distribute/of the distance from a facility to a client.
	preference_matrix preferences = preference_matrix(); // Size |I| times |J|. First element (facility) is the most preferred.
};
#endif