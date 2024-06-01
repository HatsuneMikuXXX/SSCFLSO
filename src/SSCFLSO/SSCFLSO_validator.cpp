#include "SSCFLSO_validator.h"

Validator::Validator(const SSCFLSO& instance) : ref_instance(instance){
	this->solution = facility_vector(instance.facilities, 0);
	this->solution_value = -1; 
	this->assignment = client_facility_assignment(ref_instance.clients, -1);
	this->Feasibility = alreadyComputed<bool>();
	this->Rating = alreadyComputed<double>();
}

void Validator::set_solution(const facility_vector& solution){
	this->Feasibility.aCom = false;
	this->Rating.aCom = false;
	{
		// Solution vector must be binary
		assert((solution.size() == this->ref_instance.facilities), "Solution length is incorrect");
		const std::function<bool(const int&)> is_binary([](const int& solution_entry) -> bool { return solution_entry == 0 || solution_entry == 1; });
		assert(std::all_of(std::begin(solution), std::end(solution), is_binary));
	}
	this->solution = solution;
	if(sum(solution) == 0){
		this->Feasibility.aCom = true;
		this->Feasibility.value = false;
		this->assignment = client_facility_assignment(this->ref_instance.clients, -1);
		this->solution_value = -1;
		return;
	}

	// Determine assignment.
	const preference_matrix& preferences = this->ref_instance.preferences;
	const std::function<bool(const int&)> is_open([&solution](const int& facility) -> bool { return solution[facility] == 1; });
	const std::function<int(const int&)> find_most_preferred_and_open([&preferences, &is_open](const int& client) -> int {
		auto res_it = std::find_if(std::begin(preferences[client]), std::end(preferences[client]), is_open);
		return std::distance(std::begin(preferences[client]), res_it);
	});
	std::iota(std::begin(this->assignment), std::end(this->assignment), 0); // Elements are considered as clients for the transformation next
	std::transform(std::begin(this->assignment), std::end(this->assignment), std::begin(this->assignment), find_most_preferred_and_open);

	// Compute objective value.
	client_vector clients(this->ref_instance.clients);
	std::iota(std::begin(clients), std::end(clients), 0);
	const distribution_cost_matrix& distribution_costs = this->ref_instance.distribution_costs;
	const std::function<void(const int&)> add_distribution_cost([this, &distribution_costs](const int& client) -> void {
		this->solution_value += distribution_costs[this->assignment[client]][client];
	});
	
	this->solution_value = std::inner_product(std::begin(solution), std::end(solution), std::begin(this->ref_instance.facility_costs), 0.0);
	std::for_each(std::begin(clients), std::end(clients), add_distribution_cost);
	return;
}

facility_vector Validator::exceeds_capacity(){
	facility_vector capacity_exceeding_facilities(this->ref_instance.facilities, 0);
	capacity_vector capacities = this->ref_instance.capacities;
	client_vector clients(this->ref_instance.clients);
	std::iota(std::begin(clients), std::end(clients), 0);

	// Reduce each capacity by the assigned demands. 
	const std::function<void(const int&)> reduce_capacity([&capacities, this](const int& client) -> void {
		capacities[this->assignment[client]] -= this->ref_instance.demands[client];
	});
	std::for_each(std::begin(clients), std::end(clients), reduce_capacity);
	// Determine facilities who cannot serve that much demand
	const std::function<bool(const double&)> is_negative([](const double& value) -> bool { return value < 0; });
	auto cap_it = std::begin(capacities);
	while (cap_it != std::end(capacities)) {
		cap_it = std::find_if(cap_it, std::end(capacities), is_negative);
		if (cap_it != std::end(capacities)) {
			capacity_exceeding_facilities[std::distance(std::begin(capacities), cap_it)] = 1;
		}
		cap_it++;
	}
	int sum_of_capacity_exceeding_facilities = sum(capacity_exceeding_facilities);
	this->Feasibility.aCom = true;
	this->Feasibility.value = sum_of_capacity_exceeding_facilities == 0;
	this->Rating.aCom = true;
	this->Rating.value = sum_of_capacity_exceeding_facilities;
	return capacity_exceeding_facilities;
}

void Validator::drop_empty_facilities() {
	int facility_id = 0;
	while (facility_id < this->ref_instance.facilities) {
		if (std::find(std::begin(this->assignment), std::end(this->assignment), facility_id) == std::end(this->assignment)) {
			this->solution[facility_id] = 0;
		}
		facility_id++;
	}
	this->set_solution(this->solution);
}

double Validator::evaluate_inf_solution() {
	return (this->Rating.aCom) ? this->Rating.value : sum(this->exceeds_capacity());
}


const facility_vector& Validator::get_solution() {
	return this->solution;
}

const client_facility_assignment& Validator::get_assignment() {
	return this->assignment;
}

double Validator::value() {
	return this->solution_value;
}

bool Validator::feasible() {
	return (this->Feasibility.aCom) ? this->Feasibility.value : sum(this->exceeds_capacity()) == 0;
}