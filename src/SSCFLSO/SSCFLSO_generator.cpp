#include "SSCFLSO_generator.h"

Generator::Generator(int J, int I){
	this->instance.facilities = J;
	this->instance.clients = I;
	this->instance.demands = std::vector<float>(I, 0);
	this->instance.capacities = std::vector<float>(J, 0);
	this->instance.open_costs = std::vector<float>(J, 0);
	this->instance.dist_costs = std::vector<std::vector<float>>(J, std::vector<float>(I, 0));
	this->instance.preferences = std::vector<std::vector<int>>(I, std::vector<int>(J, -1));
}

void Generator::set_demand(int client, float demand){
	this->instance.demands[client] = demand;
}

void Generator::set_capacity(int facility, float capacity){
	this->instance.capacities[facility] = capacity;
}

void Generator::set_open_cost(int facility, float open_cost){
	this->instance.open_costs[facility] = open_cost;
}

void Generator::set_dist_cost(int facility, int client, float dist_cost){
	this->instance.dist_costs[facility][client] = dist_cost;
}

void Generator::set_preferences(int category){
	int I = this->instance.clients;
	std::vector<int> clients = range(I);
	int J = this->instance.facilities;
	std::vector<int> facilities = range(J);
	auto sort_by_second = [](std::pair<int, float> pair1, std::pair<int, float> pair2){ return (pair1.second < pair2.second); };
	switch(category){
		case 0:
			// Preferences = Distance
			for(auto client_it = clients.begin(); client_it != clients.end(); client_it++){
				std::vector<std::pair<int, float>> facility_dist_pairs = std::vector<std::pair<int, float>>();
				for(auto facility_it = facilities.begin(); facility_it != facilities.end(); facility_it++){
					float dist = this->instance.dist_costs[*facility_it][*client_it];
					facility_dist_pairs.push_back(std::pair<int, float>(*facility_it, dist));
				}
				sort(facility_dist_pairs.begin(), facility_dist_pairs.end(), sort_by_second);
				this->instance.preferences[*client_it] = get_firsts<int, float>(facility_dist_pairs);
			}
			break;
		case 1:
			// Preference = Pertubated distance (draw from triangular distribution)
			for(auto client_it = clients.begin(); client_it != clients.end(); client_it++){
				float min_dist_of_client = this->instance.dist_costs[0][*client_it];
				float max_dist_of_client = this->instance.dist_costs[0][*client_it];
				// Compute minimum and maximum dist cost of client
				for(auto facility_it = facilities.begin(); facility_it != facilities.end(); facility_it++){
					float next_dist_costs = this->instance.dist_costs[*facility_it][*client_it];
					if(min_dist_of_client > next_dist_costs){
						min_dist_of_client = next_dist_costs;
					}
					else if(max_dist_of_client < next_dist_costs){
						max_dist_of_client = next_dist_costs;
					}
				}
				std::vector<std::pair<int, float>> facility_dist_pairs = std::vector<std::pair<int, float>>();
				// Get triangular distribution fake costs
				for(auto facility_it = facilities.begin(); facility_it != facilities.end(); facility_it++){
					float true_cost = this->instance.dist_costs[*facility_it][*client_it];
					float dist = triangular(min_dist_of_client, max_dist_of_client, true_cost);
					facility_dist_pairs.push_back(std::pair<int, float>(*facility_it, dist));
				}
				sort(facility_dist_pairs.begin(), facility_dist_pairs.end(), sort_by_second);
				this->instance.preferences[*client_it] = get_firsts<int, float>(facility_dist_pairs);
			}
			break;
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
		line4 += std::to_string(ref_instance.open_costs[facility]) + "\t";
	}
	// Distance/Distribution costs
	for(int facility = 0; facility < J; facility++){
		for(int client = 0; client < I; client++){
			line5 += std::to_string(ref_instance.dist_costs[facility][client]) + "\t";
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
	// Parse data - Extract numbers (including `.` for floating points) only.
	char current_character;
	std::string symbol = "";
	std::vector<float> data = std::vector<float>();
	while(file){
		file.get(current_character);
		// 48 = '0', 57 = '9', and 46 = '.'
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
		instance.open_costs.push_back(data[access]);
	}
	index += J;
	// Distribution costs
	for(int facility = 0; facility < J; facility++){
		std::vector<float> dist_costs_of_facility = std::vector<float>();
		for(int access = (index + facility * I); access < (index + (facility + 1) * I); access++){
			dist_costs_of_facility.push_back(data[access]);
		}
		instance.dist_costs.push_back(dist_costs_of_facility);
	}
	index += J * I;
	// Preferences
	if(!preferences_included){
		auto sort_by_second = [](std::pair<int, float> pair1, std::pair<int, float> pair2){ return (pair1.second < pair2.second); };
		for(int client = 0; client < I; client++){
			std::vector<std::pair<int, float>> facility_dist_pairs = std::vector<std::pair<int, float>>();
			for(int facility = 0; facility < J; facility++){
				float dist = instance.dist_costs[facility][client];
				facility_dist_pairs.push_back(std::pair<int, float>(facility, dist));
			}
			sort(facility_dist_pairs.begin(), facility_dist_pairs.end(), sort_by_second);
			instance.preferences.push_back(get_firsts<int, float>(facility_dist_pairs)); 
		}
	}
	else{
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
	int J = this->instance.facilities;
	int I = this->instance.clients;
	this->instance.demands = std::vector<float>(I, 0);
	this->instance.capacities = std::vector<float>(J, 0);
	this->instance.open_costs = std::vector<float>(J, 0);
	this->instance.dist_costs = std::vector<std::vector<float>>(J, std::vector<float>(I, 0));
	this->instance.preferences = std::vector<std::vector<int>>(I, std::vector<int>(J, 0));
	auto u = []{return uniform(0, 1);};
	std::vector<std::pair<float, float>> clients = std::vector<std::pair<float, float>>();
	std::vector<std::pair<float, float>> facilities = std::vector<std::pair<float, float>>();
	// Demands
	for(int client = 0; client < I; client++){
		clients.push_back(std::pair<float, float>(u(), u()));
		this->instance.demands[client] = uniform(5, 35);
	}
	// Preliminary capacity cost
	std::vector<float> prelim_capacities = std::vector<float>();
	for(int facility = 0; facility < J; facility++){
		facilities.push_back(std::pair<float, float>(u(), u()));
		// Dist costs
		for(int client = 0; client < I; client++){
			float c_x = clients[client].first;
			float c_y = clients[client].second;
			float f_x = facilities[facility].first;
			float f_y = facilities[facility].second;
			this->instance.dist_costs[facility][client] = 10 * sqrt( (pow(c_x - f_x, 2)) * (pow(c_y - f_y, 2)) );
		}
		prelim_capacities.push_back(uniform(10, 160)); 
	}
	float sum_of_capacities = accumulate(prelim_capacities.begin(), prelim_capacities.end(), 0);
	float sum_of_demands = accumulate(this->instance.demands.begin(), this->instance.demands.end(), 0);
	float ratio = sum_of_capacities/sum_of_demands;
	// Actual capacity and opening cost
	for(int facility = 0; facility < J; facility++){
		float capacity = ratio * prelim_capacities[facility];
		this->instance.capacities[facility] = capacity;
		this->instance.open_costs[facility] = uniform(0, 90) + uniform(100, 110) * sqrt(capacity);
	}
	this->set_preferences(0);
	if(!filename.empty()){
		this->save_instance(this->instance, filename, true);
	}
}