#include "SSCFLSO_generator.h"

/**
 * Responsible for creating SSCFLSO instances, saving instances as files, and loading instances from files.
*/

Generator::Generator(int J, int I){
	this->instance.facilities = J;
	this->instance.clients = I;
	this->instance.demands = demand_vector(I, 0);
	this->instance.capacities = capacity_vector(J, 0);
	this->instance.facility_costs = facility_cost_vector(J, 0);
	this->instance.distribution_costs = distribution_cost_matrix(J, std::vector<double>(I, 0));
	this->instance.preferences = preference_matrix(I, std::vector<int>(J, -1));
}

void Generator::set_preferences(Category category){
	// Sort facilities by the preference order
	std::vector<std::pair<int, double>> facility_distribution_cost_pairs;

	switch(category){
		case cooperative:
			// Preferences = Shortest distance is most preferred
			for (int i = 0; i < instance.clients; i++) {
				facility_distribution_cost_pairs = std::vector<std::pair<int, double>>();
				for (int j = 0; j < instance.facilities; j++) {
					double dist = this->instance.distribution_costs[j][i];
					bisect_insert(facility_distribution_cost_pairs, std::pair<int, double>(j, dist), sort_by_second);
				}
				this->instance.preferences[i] = projection_1_2<int, double>(facility_distribution_cost_pairs);
			}
			return;
		case linear_bias:
			// Preference = Pertubated distance (draw from triangular distribution)
			for (int i = 0; i < instance.clients; i++) {
				// Initial values, they are adjusted in the loop later
				double min_dist_of_client = this->instance.distribution_costs[0][i];
				double max_dist_of_client = this->instance.distribution_costs[0][i];
				// Compute minimum and maximum dist cost of client
				for (int j = 1; j < instance.facilities; j++) {
					double next_dist_costs = this->instance.distribution_costs[j][i];
					if(min_dist_of_client > next_dist_costs){
						min_dist_of_client = next_dist_costs;
					}
					else if(max_dist_of_client < next_dist_costs){
						max_dist_of_client = next_dist_costs;
					}
				}
				facility_distribution_cost_pairs = std::vector<std::pair<int, double>>();
				// Use minimum and maximum as well as the true distance to draw fake costs from a triangular distribution
				for (int j = 0; j < instance.facilities; j++) {
					double true_cost = this->instance.distribution_costs[j][i];
					double dist = triangular(min_dist_of_client, max_dist_of_client, true_cost);
					bisect_insert(facility_distribution_cost_pairs, std::pair<int, double>(j, dist), sort_by_second);
				}
				this->instance.preferences[i] = projection_1_2<int, double>(facility_distribution_cost_pairs);
			}
			return;
		default:
			throw std::runtime_error("Category" + std::to_string(category) + " is not defined.");
	}
}



void Generator::save_instance(const SSCFLSO& ref_instance, const std::string& filename, bool overwrite){
	// Check if we can write the file in the first place.
	if(!overwrite){
		std::ifstream file;
		file.open(filename);
		if(file){
			file.close();
			return;
		}
		file.close();
	}
	// Put data into the corresponding strings
	int J = ref_instance.facilities;
	int I = ref_instance.clients;
	std::ofstream out(filename);
	out << std::to_string(J) + "\t" + std::to_string(I) << "\n\n";
	std::string line5 = "";
	std::string line6 = "";
	// Demands
	for(int client = 0; client < I; client++){
		out << std::to_string(ref_instance.demands[client]) + "\t";
	}
	out << "\n\n";
	// Capacities
	for(int facility = 0; facility < J; facility++){
		out << std::to_string(ref_instance.capacities[facility]) + "\t";
	}
	out << "\n\n";
	// Opening costs
	for(int facility = 0; facility < J; facility++){
		out << std::to_string(ref_instance.facility_costs[facility]) + "\t";
	}
	out << "\n\n";
	// Distance/Distribution costs
	for(int facility = 0; facility < J; facility++){
		for(int client = 0; client < I; client++){
			line5 += std::to_string(ref_instance.distribution_costs[facility][client]) + "\t";
		}
		line5 += "\n";
	}
	out << line5 << "\n";
	// Preferences
	for(int client = 0; client < I; client++){
		for(int facility = 0; facility < J; facility++){
			line6 += std::to_string(ref_instance.preferences[client][facility]) + "\t";
		}
		line6 += "\n";
	}
	line6.pop_back();
	out << line6;
	out.close();
}

SSCFLSO Generator::load_instance(const std::string& filename, bool preferences_included, Category category){
	std::ifstream file(filename);
	if(!file){
		throw std::runtime_error("SSCFLSO Generator: Load failed. Cannot find file: " + filename);
	}
	// Parse data - Extract numbers (including `.` for floating points) only.
	char current_character;
	std::string symbol = "";
	std::vector<double> data = std::vector<double>();
	while(file){
		file.get(current_character);
		// integer value/char: 48 = '0', 57 = '9', and 46 = '.'
		if((current_character >= 48 && current_character <= 57) || current_character == 46){
			symbol += current_character; 
		}
		else if(!symbol.empty()){
			data.push_back(stof(symbol));
			symbol = "";
		}
	}
	if (symbol != "") {
		data.push_back(stof(symbol));
	}
	// Construct instance
	int J = int(data[0]);
	int I = int(data[1]);

	// Quick assertion
	if (preferences_included) {
		assert(data.size() == 2 + I + 2*J + 2*I*J);
	}
	else {
		assert(data.size() == 2 + I + 2 * J + I * J);
	}

	Generator res(J, I);
	int counter = 0;
	int index = 2;
	// Demands
	for(int access = index; access < index + I; access++){
		res.set_demand(counter, data[access]);
		counter++;
	}
	counter = 0;
	index += I;
	// Capacities
	for(int access = index; access < index + J; access++){
		res.set_capacity(counter, data[access]);
		counter++;
	}
	counter = 0;
	index += J;
	// Open costs
	for(int access = index; access < index + J; access++){
		res.set_facility_cost(counter, data[access]);
		counter++;
	}
	counter = 0;
	index += J;
	// Distribution costs
	for(int facility = 0; facility < J; facility++){
		for(int access = (index + facility * I); access < (index + (facility + 1) * I); access++){
			res.set_distribution_cost(facility, counter, data[access]);
			counter++;
		}
		counter = 0;
	}
	index += J * I;
	// Preferences
	if(!preferences_included){
		res.set_preferences(category);
	}
	else{
		// Read in given preferences
		for(int client = 0; client < I; client++){
			std::vector<int> preferences_of_client = std::vector<int>();
			// Maybe here a cast problem, we'll see
			for(int access = (index + client * J); access < (index + (client + 1) * J); access++){
				preferences_of_client.push_back(data[access]);
			}
			res.instance.preferences[client] = preferences_of_client;
		}
	}
	return res.instance;
}

void Generator::i300(const std::string& filename){
	// Create own i300 instances
	int J = this->instance.facilities;
	int I = this->instance.clients;
	this->instance.demands = std::vector<double>(I, 0);
	this->instance.capacities = std::vector<double>(J, 0);
	this->instance.facility_costs = std::vector<double>(J, 0);
	this->instance.distribution_costs = std::vector<std::vector<double>>(J, std::vector<double>(I, 0));
	this->instance.preferences = std::vector<std::vector<int>>(I, std::vector<int>(J, 0));
	auto u = []{return uniform(0, 1);};
	std::vector<std::pair<double, double>> clients = std::vector<std::pair<double, double>>();
	std::vector<std::pair<double, double>> facilities = std::vector<std::pair<double, double>>();
	// Demands
	for(int client = 0; client < I; client++){
		clients.push_back(std::pair<double, double>(u(), u()));
		this->instance.demands[client] = uniform(5, 35);
	}
	// Preliminary capacity cost
	std::vector<double> prelim_capacities = std::vector<double>();
	for(int facility = 0; facility < J; facility++){
		facilities.push_back(std::pair<double, double>(u(), u()));
		// Dist costs
		for(int client = 0; client < I; client++){
			double c_x = clients[client].first;
			double c_y = clients[client].second;
			double f_x = facilities[facility].first;
			double f_y = facilities[facility].second;
			this->instance.distribution_costs[facility][client] = 10 * sqrt( (pow(c_x - f_x, 2)) * (pow(c_y - f_y, 2)) );
		}
		prelim_capacities.push_back(uniform(10, 160)); 
	}
	double sum_of_capacities = std::accumulate(prelim_capacities.begin(), prelim_capacities.end(), 0);
	double sum_of_demands = std::accumulate(this->instance.demands.begin(), this->instance.demands.end(), 0);
	double ratio = sum_of_capacities/sum_of_demands;
	// Actual capacity and opening cost
	for(int facility = 0; facility < J; facility++){
		double capacity = ratio * prelim_capacities[facility];
		this->instance.capacities[facility] = capacity;
		this->instance.facility_costs[facility] = uniform(0, 90) + uniform(100, 110) * sqrt(capacity);
	}
	if(!filename.empty()){
		this->save_instance(this->instance, filename, true);
	}
}

// Setter and Getter
void Generator::set_demand(int client, double demand) {
	this->instance.demands[client] = demand;
}

void Generator::set_capacity(int facility, double capacity) {
	this->instance.capacities[facility] = capacity;
}

void Generator::set_facility_cost(int facility, double facility_cost) {
	this->instance.facility_costs[facility] = facility_cost;
}

void Generator::set_distribution_cost(int facility, int client, double distribution_cost) {
	this->instance.distribution_costs[facility][client] = distribution_cost;
}

SSCFLSO Generator::get_instance() {
	return this->instance;
}