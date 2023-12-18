#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include "SSCFLSO.h"
#include "../helpers/data_helper.cpp"
#include "../helpers/random.h"


class Generator{
	public:
		Generator(int J, int I);
		void set_demand(int client, float demand);
		void set_capacity(int facility, float capacity);
		void set_open_cost(int facility, float open_cost);
		void set_dist_cost(int facility, int client, float dist_cost);
		void set_preferences(int category);

		SSCFLSO get_instance();
		static void save_instance(SSCFLSO* ptr_instance, string filename, bool overwrite = false);
		static SSCFLSO load_instance(string filename, bool preferences_included = true);
		void i300(string filename);
	private:
		SSCFLSO instance;
};