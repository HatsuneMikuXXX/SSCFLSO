#ifndef GENERATOR_H
#define GENERATOR_H
#include "../common.h"
#include "../helpers/utility.h"
#include "../helpers/random.h"
#include "SSCFLSO.h"
class Generator{
	public:
		Generator(const int number_of_facilities, const int number_of_clients);
		const SSCFLSO& get_instance();
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