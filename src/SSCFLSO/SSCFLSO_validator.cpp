#include "SSCFLSO_validator.h"

/**
 * Responsible for checking solutions for feasibility and computing solution values
*/

Validator::Validator(const SSCFLSO& instance) : ref_instance(instance){
	this->solution = facility_vector(instance.facilities, 0);
	this->solution_value = -1; 
	this->assignment = client_facility_assignment(ref_instance.clients, -1);
}

void Validator::set_solution(const facility_vector& solution){
	assert((solution.size() == this->ref_instance.facilities)); // Solution vector must be binary
	this->solution = solution;
	int n = sum(solution);
	if(n == 0){
		this->assignment = client_facility_assignment(this->ref_instance.clients, -1);
		this->solution_value = -1;
		return;
	}
	// Determine assignment.
	for (int i = 0; i < this->ref_instance.clients; i++) {
		const facility_vector& ref_preferences = this->ref_instance.preferences[i];
		for(auto facility_it = ref_preferences.begin(); facility_it != ref_preferences.end(); facility_it++){
			if(this->solution[*facility_it] == 1){
				this->assignment[i] = *facility_it;
				break;
			}
			else if (this->solution[*facility_it] != 0) {
				assert(false);
			}
		}
	}
	// Compute objective value.
	this->solution_value = std::inner_product(solution.begin(), solution.end(), this->ref_instance.facility_costs.begin(), 0.0);
	for (int i = 0; i < this->ref_instance.clients; i++) {
		int facility = this->assignment[i];
		this->solution_value += this->ref_instance.distribution_costs[facility][i];
	}
}

facility_vector Validator::get_solution() {
	return this->solution;
}

client_facility_assignment Validator::get_assignment(){
	return this->assignment;
}

double Validator::value() const{
	return this->solution_value;
}

bool Validator::feasible(){
	return (sum(this->solution) == 0) ? false : sum(this->exceeds_capacity()) == 0;
}

facility_vector Validator::exceeds_capacity(){
	facility_vector capacity_exceeding_facilities = facility_vector(this->ref_instance.facilities, 0);
	capacity_vector capacities = this->ref_instance.capacities;
	// Reduce each "capacity" by the assigned demands. 
	for (int i = 0; i < this->ref_instance.clients; i++) {
		int facility = this->assignment[i];
		capacities[facility] -= this->ref_instance.demands[i];
		if (capacities[facility] < 0) {
			capacity_exceeding_facilities[facility] = 1;
		}
	}
	return capacity_exceeding_facilities;
}

void Validator::drop_empty_facilities() {
	if (this->solution_value == -1) { return; }
	for (int j = 0; j < this->ref_instance.facilities; j++) {
		if (this->solution[j] == 0) { continue; }
		else if (this->solution[j] != 1) { assert(false); }
		if (!contains(this->assignment, j)) {
			// Facility is empty
			this->solution[j] = 0;
			this->solution_value -= this->ref_instance.facility_costs[j];
		}
	}
}