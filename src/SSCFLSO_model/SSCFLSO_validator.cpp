#include "SSCFLSO_validator.h"

Validator::Validator(const SSCFLSO& instance) : ref_instance(instance){
	solution = facility_vector(instance.facilities, 0);
	assignment = client_facility_assignment(instance.clients, -1);
}

void Validator::set_solution(const facility_vector& input_solution){
	if (solution == input_solution) { return; }

	feasibility_already_computed = false;
	infeasible_solution_rating_already_computed = false;
	{
		// Solution vector must be binary
		assert(((solution.size() == ref_instance.facilities), "Solution length is incorrect"));
		assert(asa::all_of(solution, [](const int b) -> bool { return b == 0 || b == 1; }));
	}
	solution = input_solution;
	if (asa::all_of(solution, [](const int facility_open) -> bool { return facility_open == 0; })) {
		feasibility_already_computed = true;
		is_feasible = false;
		assignment = client_facility_assignment(ref_instance.clients, -1);
		solution_value = -1;
		return;
	}

	// Determine assignment.
	const preference_matrix& preferences = ref_instance.preferences;
	const std::function<bool(const int)> is_open([&input_solution](const int facility_id) -> bool { return input_solution[facility_id] == 1; });
	int client_id = 0;
	const std::function<int()> find_most_preferred_and_open([&client_id, &preferences, &is_open]() -> int {
		const auto res_it = std::find_if(std::begin(preferences[client_id]), std::end(preferences[client_id]), is_open);
		client_id++;
		return *res_it;
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
			capacity_exceeding_facilities[std::distance(std::begin(capacities), cap_it++)] = 1;
		}
	}
	const int sum_of_capacity_exceeding_facilities = asa::sum(capacity_exceeding_facilities, 0);
	feasibility_already_computed = true;
	is_feasible = sum_of_capacity_exceeding_facilities == 0;
	infeasible_solution_rating_already_computed = true;
	infeasible_solution_rating = sum_of_capacity_exceeding_facilities;
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
	return (infeasible_solution_rating_already_computed) ? infeasible_solution_rating : asa::sum(exceeds_capacity(), 0);
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
	return (feasibility_already_computed) ? is_feasible : asa::sum(exceeds_capacity(), 0) == 0;
}


SolutionContainer::SolutionContainer(const facility_vector& root) : root(root), tail(root) {}

void SolutionContainer::add(const facility_vector& next) {
	assert(next.size() == root.size());
	int index = 0;
	bool no_change = true;
	asa::for_each(next, [this, &index, &no_change](const bool facility_is_open) {
		if (facility_is_open != tail[index]) {
			change_log.push_back((index + 1));
			tail[index++] = facility_is_open;
			no_change = false;
		}
		else {
			index++;
		}
		});
	if (!no_change) {
		change_log.back() *= -1;
	}
}

bool SolutionContainer::contains(const facility_vector& val) const {
	assert(val.size() == root.size());
	// Compute difference vector and hamming distance
	std::vector<bool> d(val.size());
	int hd = 0;

	int index = 0;
	asa::generate(d, [this, &val, &index, &hd]() -> bool {
		if (root[index] != val[index]) {
			index++;
			hd++;
			return true;
		}
		index++;
		return false;
	});
	// Do the comparison
	if (hd == 0) {
		return true;
	}
	index = 0;
	while (index < change_log.size()) {
		int log_entry = change_log[index++];
		int true_index = (log_entry < 0) ? (-log_entry) - 1 : log_entry - 1;
		hd += d[true_index] ? -1 : 1;
		if (log_entry < 0 && hd == 0) {
			return true;
		}
		d[true_index] = !d[true_index];
	}
	return false;
}