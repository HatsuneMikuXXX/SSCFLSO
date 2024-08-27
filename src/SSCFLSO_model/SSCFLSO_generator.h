#ifndef GENERATOR_H
#define GENERATOR_H
#include "../common.h"
#include "../helpers/utility.h"
#include "../helpers/random.h"
#include <fstream>
#include <iostream>
#include <cassert>

typedef std::vector<double> demand_vector;
typedef std::vector<double> capacity_vector;
typedef std::vector<double> facility_cost_vector;
typedef std::vector<double> facility_distribution_cost_vector;
typedef std::vector<facility_distribution_cost_vector> distribution_cost_matrix;
typedef std::vector<int> client_preference_vector;
typedef std::vector<client_preference_vector> preference_matrix;

struct SSCFLSO {
	int facilities = 0; // Number of facilities = |J|. Identifiers zero-based.
	int clients = 0; // Number of clients = |I|. Identifiers zero-based.
	demand_vector demands = demand_vector(); // Size |I|.
	capacity_vector capacities = capacity_vector(); // Size |J|.
	facility_cost_vector facility_costs = facility_cost_vector(); // Size |J|. Cost to open a facility.
	distribution_cost_matrix distribution_costs = distribution_cost_matrix(); // Size |J| times |I|. Cost to distribute/of the distance from a facility to a client.
	preference_matrix preferences = preference_matrix(); // Size |I| times |J|. First element (facility) is the most preferred.
};

class Generator{
	public:
		Generator(const int number_of_facilities, const int number_of_clients);
		const SSCFLSO& get_instance() const;
		void set_demand(const int client, const double demand);
		void set_capacity(const int facility, const double capacity);
		void set_facility_cost(const int facility, const double facility_cost);
		void set_distribution_cost(const int facility, const int client, const double distribution_cost);
		enum Category { closest_assignment, perturbed_closest_assignment, farthest_assignment };
		void set_preferences(const Category category);

		static void save_instance(const SSCFLSO& ref_instance, const std::string& path, const bool overwrite = false);
		static SSCFLSO load_instance(const std::string& path, const bool preferences_included = false, const Category category = closest_assignment);
	private:
		SSCFLSO instance;
};
#endif