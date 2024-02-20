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

void Generator::set_demand(int client, double demand){
	this->instance.demands[client] = demand;
}

void Generator::set_capacity(int facility, double capacity){
	this->instance.capacities[facility] = capacity;
}

void Generator::set_facility_cost(int facility, double facility_cost){
	this->instance.facility_costs[facility] = facility_cost;
}

void Generator::set_distribution_cost(int facility, int client, double distribution_cost){
	this->instance.distribution_costs[facility][client] = distribution_cost;
}

void Generator::set_preferences(Category category){
	std::vector<int> clients = range(this->instance.clients);
	std::vector<int> facilities = range(this->instance.facilities); // Not binary!


	std::vector<std::pair<int, double>> facility_distribution_cost_pairs = std::vector<std::pair<int, double>>();
	switch(category){
		case cooperative:
			// Preferences = Shortest distance is most preferred
			for(auto client_it = clients.begin(); client_it != clients.end(); client_it++){
				facility_distribution_cost_pairs = std::vector<std::pair<int, double>>();
				for(auto facility_it = facilities.begin(); facility_it != facilities.end(); facility_it++){
					double dist = this->instance.distribution_costs[*facility_it][*client_it];
					bisect_insert(facility_distribution_cost_pairs, std::pair<int, double>(*facility_it, dist), sort_by_second);
				}
				this->instance.preferences[*client_it] = get_firsts<int, double>(facility_distribution_cost_pairs);
			}
			return;

		case linear_bias:
			// Preference = Pertubated distance (draw from triangular distribution)
			for(auto client_it = clients.begin(); client_it != clients.end(); client_it++){
				double min_dist_of_client = this->instance.distribution_costs[0][*client_it];
				double max_dist_of_client = this->instance.distribution_costs[0][*client_it];
				// Compute minimum and maximum dist cost of client
				for(auto facility_it = facilities.begin(); facility_it != facilities.end(); facility_it++){
					double next_dist_costs = this->instance.distribution_costs[*facility_it][*client_it];
					if(min_dist_of_client > next_dist_costs){
						min_dist_of_client = next_dist_costs;
					}
					else if(max_dist_of_client < next_dist_costs){
						max_dist_of_client = next_dist_costs;
					}
				}
				facility_distribution_cost_pairs = std::vector<std::pair<int, double>>();
				// Use minimum and maximum as well as the true distance to draw fake costs from a triangular distribution
				for(auto facility_it = facilities.begin(); facility_it != facilities.end(); facility_it++){
					double true_cost = this->instance.distribution_costs[*facility_it][*client_it];
					double dist = triangular(min_dist_of_client, max_dist_of_client, true_cost);
					bisect_insert(facility_distribution_cost_pairs, std::pair<int, double>(*facility_it, dist), sort_by_second);
				}
				this->instance.preferences[*client_it] = get_firsts<int, double>(facility_distribution_cost_pairs);
			}
			return;
		default:
			throw std::runtime_error("Category" + std::to_string(category) + " is not defined.");
	}
}

SSCFLSO Generator::get_instance(){
	return this->instance;
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
	int J = ref_instance.facilities;
	int I = ref_instance.clients;
	std::string line1 = std::to_string(J) + "\t" + std::to_string(I);
	std::string line2 = "";
	std::string line3 = "";
	std::string line4 = "";
	std::string line5 = "";
	std::string line6 = "";
	// Demands
	for(int client = 0; client < I; client++){
		line2 += std::to_string(ref_instance.demands[client]) + "\t";
	}
	// Capacities
	for(int facility = 0; facility < J; facility++){
		line3 += std::to_string(ref_instance.capacities[facility]) + "\t";
	}
	// Opening costs
	for(int facility = 0; facility < J; facility++){
		line4 += std::to_string(ref_instance.facility_costs[facility]) + "\t";
	}
	// Distance/Distribution costs
	for(int facility = 0; facility < J; facility++){
		for(int client = 0; client < I; client++){
			line5 += std::to_string(ref_instance.distribution_costs[facility][client]) + "\t";
		}
		line5 += "\n";
	}
	line5.pop_back();
	// Preferences
	for(int client = 0; client < I; client++){
		for(int facility = 0; facility < J; facility++){
			line6 += std::to_string(ref_instance.preferences[client][facility]) + "\t";
		}
		line6 += "\n";
	}
	line6.pop_back();
	std::string result = line1 + "\n\n" + line2 + "\n\n" + line3 + "\n\n" + line4 + "\n\n" + line5 + "\n\n" + line6;
	std::ofstream out(filename);
	out << result;
	out.close();
}

SSCFLSO Generator::load_instance(const std::string& filename, bool preferences_included){
	std::ifstream file(filename);
	if(!file){
		throw std::runtime_error("Load failed. Cannot find file: " + filename);
	}
	// Parse data - Extract numbers (including `.` for doubleing points) only.
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
	// Construct solution
	SSCFLSO instance;
	int index = 2;
	int J = int(data[0]);
	int I = int(data[1]);
	instance.facilities = J;
	instance.clients = I;
	// Demands
	for(int access = index; access < index + I; access++){
		instance.demands.push_back(data[access]);
	}
	index += I;
	// Capacities
	for(int access = index; access < index + J; access++){
		instance.capacities.push_back(data[access]);
	}
	index += J;
	// Open costs
	for(int access = index; access < index + J; access++){
		instance.facility_costs.push_back(data[access]);
	}
	index += J;
	// Distribution costs
	for(int facility = 0; facility < J; facility++){
		std::vector<double> dist_costs_of_facility = std::vector<double>();
		for(int access = (index + facility * I); access < (index + (facility + 1) * I); access++){
			dist_costs_of_facility.push_back(data[access]);
		}
		instance.distribution_costs.push_back(dist_costs_of_facility);
	}
	index += J * I;
	// Preferences
	if(!preferences_included){
		// Create preferences using the data - cooperative
		for(int client = 0; client < I; client++){
			std::vector<std::pair<int, double>> facility_dist_pairs = std::vector<std::pair<int, double>>();
			for(int facility = 0; facility < J; facility++){
				double dist = instance.distribution_costs[facility][client];
				bisect_insert(facility_dist_pairs, std::pair<int, double>(facility, dist), sort_by_second);
			}
			instance.preferences.push_back(get_firsts<int, double>(facility_dist_pairs)); 
		}
	}
	else{
		// Read in given preferences
		for(int client = 0; client < I; client++){
			std::vector<int> preferences_of_client = std::vector<int>();
			// Maybe here a cast problem, we'll see
			for(int access = (index + client * J); access < (index + (client + 1) * J); access++){
				preferences_of_client.push_back(data[access]);
			}
			instance.preferences.push_back(preferences_of_client);
		}
	}
	return instance;
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
	this->set_preferences(cooperative);
	if(!filename.empty()){
		this->save_instance(this->instance, filename, true);
	}
}