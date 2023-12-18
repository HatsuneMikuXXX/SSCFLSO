#include "SSCFLSO_generator.h"

Generator::Generator(int J, int I){
	this->instance.facilities = J;
	this->instance.clients = I;
	this->instance.demands = vector<float>(I, 0);
	this->instance.capacities = vector<float>(J, 0);
	this->instance.open_costs = vector<float>(J, 0);
	this->instance.dist_costs = vector<vector<float>>(J, vector<float>(I, 0));
	this->instance.preferences = vector<vector<int>>(I, vector<int>(J, -1));
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
	int J = this->instance.facilities;
	auto sort_by_second = [](pair<int, float> pair1, pair<int, float> pair2){ return (pair1.second < pair2.second); };
	switch(category){
		case 0:
			// Preferences = Distance
			for(int client = 0; client < I; client++){
				vector<pair<int, float>> facility_dist_pairs = vector<pair<int, float>>();
				for(int facility = 0; facility < J; facility++){
					float dist = this->instance.dist_costs[facility][client];
					facility_dist_pairs.push_back(pair<int, float>(facility, dist));
				}
				sort(facility_dist_pairs.begin(), facility_dist_pairs.end(), sort_by_second);
				this->instance.preferences[client] = get_firsts<int, float>(&facility_dist_pairs);
			}
			break;
		case 1:
			// Preference = Pertubated distance (draw from triangular distribution)
			for(int client = 0; client < I; client++){
				// Compute minimum and maximum dist cost of client
				float min_dist_of_client = this->instance.dist_costs[0][client];
				float max_dist_of_client = this->instance.dist_costs[0][client];
				for(int facility = 1; facility < J; facility++){
					float next_dist_costs = this->instance.dist_costs[facility][client];
					if(min_dist_of_client > next_dist_costs){
						min_dist_of_client = next_dist_costs;
					}
					else if(max_dist_of_client < next_dist_costs){
						max_dist_of_client = next_dist_costs;
					}
				}
				// Get triangular distribution fake costs
				vector<pair<int, float>> facility_dist_pairs = vector<pair<int, float>>();
				for(int facility = 0; facility < J; facility++){
					float true_cost = this->instance.dist_costs[facility][client];
					float dist = triangular(min_dist_of_client, max_dist_of_client, true_cost);
					facility_dist_pairs.push_back(pair<int, float>(facility, dist));
				}
				sort(facility_dist_pairs.begin(), facility_dist_pairs.end(), sort_by_second);
				this->instance.preferences[client] = get_firsts<int, float>(&facility_dist_pairs);
			}
			break;
		default:
			assert(false);
	}
}

SSCFLSO Generator::get_instance(){
	SSCFLSO copy;
	copy.facilities = this->instance.facilities;
	copy.clients = this->instance.clients;
	copy.demands = this->instance.demands;
	copy.capacities = this->instance.capacities;
	copy.open_costs = this->instance.open_costs;
	copy.dist_costs = this->instance.dist_costs;
	copy.preferences = this->instance.preferences;
	return copy;
}

void Generator::save_instance(SSCFLSO* ptr_instance, string filename, bool overwrite){
	// Check if we can write the file in the first place.
	if(!overwrite){
		ifstream file;
		file.open(filename);
		if(file){
			file.close();
			return;
		}
		file.close();
	}
	// Extract information and create the correct format.
	int J = ptr_instance->facilities;
	int I = ptr_instance->clients;
	string line1 = to_string(J) + "\t" + to_string(I);
	string line2 = "";
	string line3 = "";
	string line4 = "";
	string line5 = "";
	string line6 = "";
	// Demands
	for(int client = 0; client < I; client++){
		line2 += to_string(ptr_instance->demands[client]) + "\t";
	}
	// Capacities
	for(int facility = 0; facility < J; facility++){
		line3 += to_string(ptr_instance->capacities[facility]) + "\t";
	}
	// Opening costs
	for(int facility = 0; facility < J; facility++){
		line4 += to_string(ptr_instance->open_costs[facility]) + "\t";
	}
	// Distance/Distribution costs
	for(int facility = 0; facility < J; facility++){
		for(int client = 0; client < I; client++){
			line5 += to_string(ptr_instance->dist_costs[facility][client]) + "\t";
		}
		line5 += "\n";
	}
	// Preferences
	for(int client = 0; client < I; client++){
		for(int facility = 0; facility < J; facility++){
			line6 += to_string(ptr_instance->preferences[client][facility]) + "\t";
		}
		line6 += "\n";
	}
	line6.pop_back();

	string result = line1 + "\n\n" + line2 + "\n\n" + line3 + "\n\n" + line4 + "\n\n" + line5 + "\n\n" + line6;
	// Save it
	ofstream out(filename);
	out << result;
	out.close();
}

SSCFLSO Generator::load_instance(string filename, bool preferences_included){
	// Open file
	ifstream file;
	file.open(filename);
	if(!file){
		cout << "Load fail. File does not exists";
		assert(false);
	}
	// Parse data - Extract numbers only.
	char current_character;
	string symbol = "";
	vector<float> data = vector<float>();
	while(file){
		file.get(current_character);
		if((current_character >= 48 && current_character <= 57) || current_character == 46){
			// 48 = '0', 57 = '9', and 46 = '.'
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
	// General info
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
		vector<float> dist_costs_of_facility = vector<float>();
		for(int access = (index + facility * I); access < (index + (facility + 1) * I); access++){
			dist_costs_of_facility.push_back(data[access]);
		}
		instance.dist_costs.push_back(dist_costs_of_facility);
	}
	index += J * I;
	// Preferences
	if(!preferences_included){
		auto sort_by_second = [](pair<int, float> pair1, pair<int, float> pair2){ return (pair1.second < pair2.second); };
		for(int client = 0; client < I; client++){
			vector<pair<int, float>> facility_dist_pairs = vector<pair<int, float>>();
			for(int facility = 0; facility < J; facility++){
				float dist = instance.dist_costs[facility][client];
				facility_dist_pairs.push_back(pair<int, float>(facility, dist));
			}
			sort(facility_dist_pairs.begin(), facility_dist_pairs.end(), sort_by_second);
			instance.preferences[client] = get_firsts<int, float>(&facility_dist_pairs);
		}
	}
	else{
		for(int client = 0; client < I; client++){
			vector<int> preferences_of_client = vector<int>();
			// Maybe here a cast problem, we'll see
			for(int access = (index + client * J); access < (index + (client + 1) * J); access++){
				preferences_of_client.push_back(data[access]);
			}
			instance.preferences.push_back(preferences_of_client);
		}
	}
	return instance;
}

void Generator::i300(string filename = ""){
	int J = this->instance.facilities;
	int I = this->instance.clients;
	this->instance.demands = vector<float>(I, 0);
	this->instance.capacities = vector<float>(J, 0);
	this->instance.open_costs = vector<float>(J, 0);
	this->instance.dist_costs = vector<vector<float>>(J, vector<float>(I, 0));
	this->instance.preferences = vector<vector<int>>(I, vector<int>(J, 0));
	
	auto u = []{return uniform(0, 1);};
	vector<pair<float, float>> clients = vector<pair<float, float>>();
	vector<pair<float, float>> facilities = vector<pair<float, float>>();
	for(int client = 0; client < I; client++){
		clients.push_back(pair<float, float>(u(), u()));
		// Demand
		this->instance.demands[client] = uniform(5, 35);
	}
	// Preliminary capacity cost
	vector<float> prelim_capacities = vector<float>();
	for(int facility = 0; facility < J; facility++){
		facilities.push_back(pair<float, float>(u(), u()));
		for(int client = 0; client < I; client++){
			// Dist costs
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
	for(int facility = 0; facility < J; facility++){
		// Actual capacity and opening cost
		float capacity = ratio * prelim_capacities[facility];
		this->instance.capacities[facility] = capacity;
		this->instance.open_costs[facility] = uniform(0, 90) + uniform(100, 110) * sqrt(capacity);
	}
	this->set_preferences(0);
	if(!filename.empty()){
		this->save_instance(&(this->instance), filename, true);
	}
}

/*
		void i300(string filename);
*/