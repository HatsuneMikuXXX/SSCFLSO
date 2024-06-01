#include "SSCFLSO_generator.h"

/**
 * Responsible for creating SSCFLSO instances, saving instances as files, and loading instances from files.
*/

Generator::Generator(const int number_of_facilities, const int number_of_clients){
	this->instance = {
		number_of_facilities,
		number_of_clients,
		demand_vector(number_of_clients, -1),
		capacity_vector(number_of_facilities, -1),
		facility_cost_vector(number_of_facilities, -1),
		distribution_cost_matrix(number_of_facilities, std::vector<double>(number_of_clients, -1)),
		preference_matrix(number_of_clients, std::vector<int>(number_of_facilities, -1)),
	};
}

void Generator::set_preferences(const Category category){
	// Takes a score function (input: client, facility) and assigns preferences using a score (return value of score function). High scores translate to high preference
	auto assign_preferences = [this](const std::function<double(const int&, const int&)>& score_function) {
		int facility_id = 0;
		const int number_of_facilities = this->instance.facilities;
		int client_id = 0;
		const int number_of_clients = this->instance.clients;

		// Generator function for scores
		const std::function<std::pair<int, double>(const int&)> generate_scores(
			[&facility_id, &number_of_facilities, &score_function](const int& client) -> std::pair<int, double> {
				assert(facility_id < number_of_facilities);
				std::pair<int, double> facility_score_pair(facility_id, score_function(client, facility_id));
				facility_id++;
				return facility_score_pair;
			});
		
		// Predicate to order pairs
		const std::function<bool(const std::pair<int, double>&, const std::pair<int, double>&)> LEQ(
			[](const std::pair<int, double>& facility_score_a, const std::pair<int, double>& facility_score_b) -> bool {
				return facility_score_a.second <= facility_score_b.second; 
			});

		// Generator function for preferences
		const std::function<client_preference_vector()> generate_preferences([&facility_id, &client_id, &number_of_clients, &number_of_facilities, &generate_scores, &LEQ]() -> client_preference_vector {
			assert(client_id < number_of_clients);
			client_preference_vector res(number_of_facilities);
			// Generate scores
			std::vector<std::pair<int, double>> facility_score_pairs({});
			facility_id = 0;
			std::generate(std::begin(facility_score_pairs), std::end(facility_score_pairs), generate_scores);
			// Sort in ascending order
			std::sort(std::begin(facility_score_pairs), std::end(facility_score_pairs), LEQ);
			// Reverse order
			std::reverse(std::begin(facility_score_pairs), std::end(facility_score_pairs));
			// Remove scores
			std::transform(std::begin(facility_score_pairs), std::end(facility_score_pairs), std::begin(res), [](const std::pair<int, double>& p) -> int { return p.first; });
			client_id++;
			return res;
		});

		// Execution
		this->instance.preferences.clear();
		std::generate(std::begin(this->instance.preferences), std::end(this->instance.preferences), generate_preferences);
	};

	std::function<double(const int&, const int&)> score_function;
	switch(category){
		case closest_assignment:
			score_function = [this](const int& client, const int& facility) {
				return this->instance.distribution_costs[facility][client] * -1; //Remember: High value = High preference.
			};
			break;
		case perturbed_closest_assignment:
			score_function = [this](const int& client, const int& facility) {
				// Determine minimal and maximal distances
				const std::function<double(const int&)> D([this, &client](const int& facility_id) -> double { return this->instance.distribution_costs[facility_id][client]; });
				std::vector<double> client_distances(this->instance.facilities);
				std::iota(std::begin(client_distances), std::end(client_distances), 0); // Fill with 0,1,2,...
				std::transform(std::begin(client_distances), std::end(client_distances), std::begin(client_distances), D);
				double min_dist = *std::min_element(std::begin(client_distances), std::end(client_distances));
				double max_dist = *std::max_element(std::begin(client_distances), std::end(client_distances));
				double true_cost = this->instance.distribution_costs[facility][client];
				return triangular(min_dist, max_dist, true_cost) * -1; //Remember: High value = High preference.
			};
			break;
		case farthest_assignment:
			score_function = [this](const int& client, const int& facility) {
				return this->instance.distribution_costs[facility][client];
			};
			break;
		default:
			throw std::runtime_error("Category " + std::to_string(category) + " is not defined.");
	}
	assign_preferences(score_function);
}

void Generator::save_instance(const SSCFLSO& ref_instance, const std::string& path, const bool overwrite){
	// Check if we don't accidentally overwrite an existing file
	if(!overwrite && std::ifstream(path)){
		std::cout << "File already exists. Results are not saved!" << std::endl;
		return;
	}
	// Create string
	{
		std::ofstream out(path);
		int J = ref_instance.facilities;
		int I = ref_instance.clients;
		out << std::to_string(J) + "\t" + std::to_string(I) << "\n\n";
		// Demands
		for (int client = 0; client < I; client++) {
			out << std::to_string(ref_instance.demands[client]) + "\t";
		}
		out << "\n\n";
		// Capacities
		for (int facility = 0; facility < J; facility++) {
			out << std::to_string(ref_instance.capacities[facility]) + "\t";
		}
		out << "\n\n";
		// Opening costs
		for (int facility = 0; facility < J; facility++) {
			out << std::to_string(ref_instance.facility_costs[facility]) + "\t";
		}
		out << "\n\n";
		// Distance/Distribution costs
		std::string line = "";
		for (int facility = 0; facility < J; facility++) {
			for (int client = 0; client < I; client++) {
				line += std::to_string(ref_instance.distribution_costs[facility][client]) + "\t";
			}
			line += "\n";
		}
		out << line << "\n";
		// Preferences
		line = "";
		for (int client = 0; client < I; client++) {
			for (int facility = 0; facility < J; facility++) {
				line += std::to_string(ref_instance.preferences[client][facility]) + "\t";
			}
			line += "\n";
		}
		line.pop_back();
		out << line;
		out.close();
	}
}

SSCFLSO Generator::load_instance(const std::string& path, const bool preferences_included, const Category category){
	std::ifstream file(path);
	if(!file){
		throw std::runtime_error("SSCFLSO Generator: Load failed. Cannot find file: " + path);
	}
	std::vector<double> data({});
	{
		// Parse data - Extract only numerical symbols (including `.` for floating points).
		char current_character;
		std::string symbol = "";
		while (file) {
			file.get(current_character);
			// integer value/char: 48 = '0', 57 = '9', and 46 = '.'
			if ((current_character >= 48 && current_character <= 57) || current_character == 46) {
				symbol += current_character;
			}
			else if (!symbol.empty()) {
				data.push_back(stof(symbol));
				symbol = "";
			}
		}
		if (symbol != "") {
			data.push_back(stof(symbol));
		}
	}
	// Construct instance
	int J = int(data[0]);
	int I = int(data[1]);
	Generator res(J, I);
	{
		// Depending on whether preferences are included, the loaded file has a specific number of values
		// 2 for the numbers I, J; I for demands; 2*J for capacities and facility cost; I * J for distribution costs; I * J for preferences
		int correct_number_of_values = 2 + I + (2 * J) + (I * J * (preferences_included + 1));
		assert(data.size() == correct_number_of_values);
	}
	{
		int counter = 0;
		int index = 2;
		// Demands
		for (int access = index; access < index + I; access++) {
			res.set_demand(counter, data[access]);
			counter++;
		}
		counter = 0;
		index += I;
		// Capacities
		for (int access = index; access < index + J; access++) {
			res.set_capacity(counter, data[access]);
			counter++;
		}
		counter = 0;
		index += J;
		// Open costs
		for (int access = index; access < index + J; access++) {
			res.set_facility_cost(counter, data[access]);
			counter++;
		}
		counter = 0;
		index += J;
		// Distribution costs
		for (int facility = 0; facility < J; facility++) {
			for (int access = (index + facility * I); access < (index + (facility + 1) * I); access++) {
				res.set_distribution_cost(facility, counter, data[access]);
				counter++;
			}
			counter = 0;
		}
		index += J * I;
	}
	
	// Preferences
	int tmp = (2 + I + 2 * J + I * J);
	if(!preferences_included){
		res.set_preferences(category);
	}
	else{
		// Read in given preferences
		for(int client = 0; client < I; client++){
			std::vector<int> preferences_of_client = std::vector<int>();
			// Maybe here a cast problem, we'll see
			for (int access = (tmp + client * J); access < (tmp + (client + 1) * J); access++) {
				preferences_of_client.push_back(data[access]);
			}
			res.instance.preferences[client] = preferences_of_client;
		}
	}
	return res.instance;
}

// Setter and Getter
const SSCFLSO& Generator::get_instance() {
	return this->instance;
}

void Generator::set_demand(const int client, const double demand) {
	this->instance.demands[client] = demand;
}

void Generator::set_capacity(const int facility, const double capacity) {
	this->instance.capacities[facility] = capacity;
}

void Generator::set_facility_cost(const int facility, const double facility_cost) {
	this->instance.facility_costs[facility] = facility_cost;
}

void Generator::set_distribution_cost(const int facility, const int client, const double distribution_cost) {
	this->instance.distribution_costs[facility][client] = distribution_cost;
}