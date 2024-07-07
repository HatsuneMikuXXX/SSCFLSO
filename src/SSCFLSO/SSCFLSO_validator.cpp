#include "SSCFLSO_validator.h"

Validator::Validator(const SSCFLSO& instance) : ref_instance(instance){
	solution = facility_vector(instance.facilities, 0);
	assignment = client_facility_assignment(instance.clients, -1);
}

void Validator::set_solution(const facility_vector& solution){
	if (solution == solution) { return; }
	Feasibility.aCom = false;
	Rating.aCom = false;
	{
		// Solution vector must be binary
		assert((solution.size() == ref_instance.facilities), "Solution length is incorrect");
		assert(asa::all_of(solution, [](const int b) -> bool { return b == 0 || b == 1; }));
	}
	solution = solution;
	if (asa::all_of(solution, [](const int facility_open) -> bool { return facility_open == 0; })) {
		Feasibility.aCom = true;
		Feasibility.value = false;
		assignment = client_facility_assignment(ref_instance.clients, -1);
		solution_value = -1;
		return;
	}

	// Determine assignment.
	const preference_matrix& preferences = ref_instance.preferences;
	const std::function<bool(const int)> is_open([&solution](const int facility_id) -> bool { return solution[facility_id] == 1; });
	int client_id = 0;
	const std::function<int()> find_most_preferred_and_open([&client_id, &preferences, &is_open]() -> int {
		const auto res_it = std::find_if(std::begin(preferences[client_id]), std::end(preferences[client_id]), is_open);
		client_id++;
		return std::distance(std::begin(preferences[client_id]), res_it);
	});
	assignment.clear();
	assignment = client_facility_assignment(ref_instance.clients);
	asa::generate(assignment, find_most_preferred_and_open);

	// Compute objective value.
	range_vector client_range = range(ref_instance.clients);
	const distribution_cost_matrix& distribution_costs = ref_instance.distribution_costs;
	const std::function<void(const int)> add_distribution_cost([this, &distribution_costs](const int client) -> void {
		solution_value += distribution_costs[assignment[client]][client];
	});
	
	solution_value = std::inner_product(std::begin(solution), std::end(solution), std::begin(ref_instance.facility_costs), 0.0);
	asa::for_each(client_range, add_distribution_cost);
	return;
}

facility_vector Validator::exceeds_capacity(){
	facility_vector capacity_exceeding_facilities(ref_instance.facilities, 0);
	capacity_vector capacities = ref_instance.capacities;
	range_vector client_range = range(ref_instance.clients);

	// Reduce each capacity by the assigned demands. 
	const std::function<void(const int&)> reduce_capacity([&capacities, this](const int& client) -> void {
		capacities[assignment[client]] -= ref_instance.demands[client];
	});
	asa::for_each(client_range, reduce_capacity);

	// Determine facilities who cannot serve that much demand
	auto cap_it = std::begin(capacities);
	while (cap_it != std::end(capacities)) {
		cap_it = std::find_if(cap_it, std::end(capacities), [](const double value) -> bool { return value < 0; });
		if (cap_it != std::end(capacities)) {
			capacity_exceeding_facilities[std::distance(std::begin(capacities), cap_it)] = 1;
		}
		cap_it++;
	}
	const int sum_of_capacity_exceeding_facilities = asa::sum(capacity_exceeding_facilities, 0);
	Feasibility.aCom = true;
	Feasibility.value = sum_of_capacity_exceeding_facilities == 0;
	Rating.aCom = true;
	Rating.value = sum_of_capacity_exceeding_facilities;
	return capacity_exceeding_facilities;
}

void Validator::drop_empty_facilities() {
	int facility_id = 0;
	while (facility_id < ref_instance.facilities) {
		if (std::find(std::begin(assignment), std::end(assignment), facility_id) == std::end(assignment)) {
			solution[facility_id] = 0;
		}
		facility_id++;
	}
	set_solution(solution);
}

double Validator::evaluate_inf_solution() {
	return (Rating.aCom) ? Rating.value : asa::sum(exceeds_capacity(), 0);
}

const facility_vector& Validator::get_solution() const {
	return solution;
}

const client_facility_assignment& Validator::get_assignment() const {
	return assignment;
}

double Validator::value() const {
	return solution_value;
}

bool Validator::feasible() {
	return (Feasibility.aCom) ? Feasibility.value : asa::sum(exceeds_capacity(), 0) == 0;
}