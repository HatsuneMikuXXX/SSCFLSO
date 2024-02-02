#ifndef GENERATOR_H
#define GENERATOR_H
#include "SSCFLSO.h"
#include "../helpers/data_helper.h"
#include "../helpers/random.h"
#include <algorithm> // For sorting
#include <fstream> // For saving and loading.
#include <string> // For saving and loading.
#include <iostream> // For throwing the error.
class Generator{
	public:
		// Cooperative: Clients want to minimize objective function.
		// Linear bias: Clients are biased to minimize the objective function but may consider worse facilities.
		enum Category {cooperative, linear_bias};

		Generator(int J, int I);
		void set_demand(int client, float demand);
		void set_capacity(int facility, float capacity);
		void set_open_cost(int facility, float open_cost);
		void set_dist_cost(int facility, int client, float dist_cost);
		/// @brief Sets the preferences by either distance or pertubated distances.
		/// @param category determines the control flow. Currently categories 0 and 1 are available. 
		void set_preferences(Category category);
		SSCFLSO get_instance();
		/// @brief Creates a .plc file which contains the information to the SSCFLSO instance.
		/// @param ref_instance the instance.
		/// @param filename the location to save to.
		/// @param overwrite if false then we do not save if the file already exists.
		static void save_instance(const SSCFLSO& ref_instance, const std::string& filename, bool overwrite = false);
		/// @brief Scans and parse a .plc file and returns the instance.
		/// @param filename the location to load from.
		/// @param preferences_included if false then we set the preferences by distance. 
		/// @return A created instance from the information.
		static SSCFLSO load_instance(const std::string& filename, bool preferences_included = true);
		/// @brief Creates an i300 instance and sets own attributes accordingly.
		/// @param filename if empty then we do not save. 
		void i300(const std::string& filename = "");
	private:
		SSCFLSO instance;
};
#endif