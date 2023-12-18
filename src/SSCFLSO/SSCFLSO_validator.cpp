#include "SSCFLSO_validator.h"

// Validator constructor
Validator::Validator(SSCFLSO* instance) : ptr_instance(instance){
	int J = this->ptr_instance->facilities;
	int I = this->ptr_instance->clients;
	this->rankings = vector<vector<int>>(I, vector<int>(J, -1));
	for(int client = 0; client < I; client++){
		int rank = 0;
		for(int facility : this->ptr_instance->preferences[client]){
			this->rankings[client][facility] = rank;
			rank += 1;
		}
	}
	this->open_facilities = vector<int>();
	this->solution_value = -1;
	this->assignment = vector<int>(I, -1);
}

void Validator::set_solution(vector<int> solution){
	this->open_facilities = solution; // Should be a deepcopy according to stack overflow.
	// Set assignment according to preferences/rankings
	int J = this->ptr_instance->facilities;
	int I = this->ptr_instance->clients;
	int n = this->open_facilities.size();
	if(n == 0){
		this->assignment = vector<int>(I, -1);
		this->solution_value = -1;
		return;
	}
	if(pow(n, 2) - 2*n - 1 >= J){
		// Expected number of checks by iterating the preferences is smaller than the [deterministic] number of checks using rankings
		// Assumption is that the solution is chosen uniformly at random though.
		for(int client; client < I; client++){
			for(int facility : this->ptr_instance->preferences[client]){
				if(contains<int>(&(this->open_facilities), facility)){
					this->assignment[client] = facility;
					break;
				}
			}
		}
	}
	else{
		for(int client = 0; client < I; client++){
			int most_preferred = this->open_facilities[0];
			for(int facility : this->open_facilities){
				if(this->rankings[client][facility] < this->rankings[client][most_preferred]){
					// We found a more preferred facility.
					most_preferred = facility;
				}
			}
			this->assignment[client] = most_preferred;
		}
	}
	// Compute the value of the solution.
	this->solution_value = 0;
	for(int facility : this->open_facilities){
		this->solution_value += this->ptr_instance->open_costs[facility];
	}
	for(int client = 0; client < I; client++){
		int facility = this->assignment[client];
		this->solution_value += this->ptr_instance->dist_costs[facility][client];
	}
	return;
}

vector<int> Validator::get_assignment(){
	vector<int> copy = this->assignment;
	return copy;
}

float Validator::value(){
	return this->solution_value;
}

bool Validator::feasible(){
	if(this->open_facilities.size() == 0){
		return false;
	}
	// Capacity check
	int I = this->ptr_instance->clients;
	vector<float> capacities = this->ptr_instance->capacities;
	for(int client = 0; client < I; client++){
		int facility = this->assignment[client];
		capacities[facility] -= this->ptr_instance->demands[client];
		if(capacities[facility] < 0){
			return false;
		}
	}
}