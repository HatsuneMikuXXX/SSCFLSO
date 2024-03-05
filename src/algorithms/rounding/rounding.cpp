#include "rounding.h"

void Rounding::solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) {
	auto start = start_timer();
	Validator FLV = Validator(instance);
	facility_vector no_unnecessary_facilities = preprocess(instance);
	FLV.set_solution(no_unnecessary_facilities);
	// If a solution is found then linear relaxation CANNOT be infeasible
	if (!FLV.feasible()) {
		return;
	}
	// Find variables that can be eliminated
	eliminate_variables variables;
	for (int j = 0; j < instance.facilities; j++) {
		variables.facilities_keep_closed.push_back(1 - no_unnecessary_facilities[j]);
		variables.distribution_keep_closed.push_back(std::vector<int>());
		for (int i = 0; i < instance.clients; i++) {
			variables.distribution_keep_closed[j].push_back(1 - no_unnecessary_facilities[j]);
		}
	}
	// Create initial solution for the solver
	initial_solution init;
	init.initial_facilities = no_unnecessary_facilities;
	for (int j = 0; j < instance.facilities; j++) {
		init.initial_distribution.push_back(std::vector<int>(instance.clients, 0));
	}
	for (int i = 0; i < instance.clients; i++) {
		int j = FLV.get_assignment()[i];
		init.initial_distribution[j][i] = 1;
	}

	relaxed_solution rs = solve_linear_relaxation(instance, variables, &init);
	
	facility_vector solution = facility_vector(instance.facilities, 0);
	// Round up
	for (int j = 0; j < instance.facilities; j++) {
		if (rs.rFacility_vector[j] > 0) {
			solution[j] = 1;
		}
	}
	// Close empty facilities
	
	FLV.set_solution(solution);
	FLV.drop_empty_facilities();
	assert(FLV.feasible());
	if (within_time_limit(start, time_limit)) {
		current_best = solution;
	}
}

std::string Rounding::meta_information() {
	std::ifstream file(PATH + "rounding/rounding.txt");
	if (file) {
		std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		return content;
	}
	else {
		throw std::runtime_error("Rounding information file not found.");
	}
}
