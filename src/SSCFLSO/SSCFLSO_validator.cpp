#include "SSCFLSO_validator.h"

Validator::Validator(const SSCFLSO& instance) : ref_instance(instance){
	this->assign_rankings();
	this->open_facilities = std::vector<int>();
	this->solution_value = -1;
	this->assignment = std::vector<int>(ref_instance.clients, -1);
}

void Validator::assign_rankings(){
	int J = ref_instance.facilities;
	int I = ref_instance.clients;
	std::vector<int> ranks = range(J);
	std::vector<int> clients = range(I);
	this->rankings = std::vector<std::vector<int>>(I, std::vector<int>());
	for(auto it = clients.begin(); it != clients.end(); it++){
		permutated_fill(ref_instance.preferences[*it], ranks, this->rankings[*it]);
	}
}

void Validator::set_solution(const std::vector<int>& solution){
	this->open_facilities = solution;
	// Determine assignment.
	int J = this->ref_instance.facilities;
	int I = this->ref_instance.clients;
	std::vector<int> clients = range(I);
	int n = this->open_facilities.size();
	if(n == 0){
		this->assignment = std::vector<int>(I, -1);
		this->solution_value = -1;
		return;
	}
	if(pow(n, 2) - 2*n - 1 >= J){
		// Expected number of checks by iterating the preferences is smaller than the [deterministic] number of checks using rankings.
		// Assumption is that the solution is chosen uniformly at random though.
		for(auto client_it = clients.begin(); client_it != clients.end(); client_it++){
			const std::vector<int>& ref_preferences = this->ref_instance.preferences[*client_it];
			for(auto facility_it = ref_preferences.begin(); facility_it != ref_preferences.end(); facility_it++){
				if(contains(this->open_facilities, *facility_it)){
					this->assignment[*client_it] = *facility_it;
					break;
				}
			}
		}
	}
	else{
		for(auto client_it = clients.begin(); client_it != clients.end(); client_it++){
			int most_preferred = this->open_facilities[0];
			for(auto facility_it = solution.begin(); facility_it != solution.end(); facility_it++){
				if(this->rankings[*client_it][*facility_it] < this->rankings[*client_it][most_preferred]){
					most_preferred = *facility_it;
				}
			}
			this->assignment[*client_it] = most_preferred;
		}
	}
	// Compute objective value.
	this->solution_value = 0;
	for(auto it = solution.begin(); it != solution.end(); it++){
		this->solution_value += this->ref_instance.open_costs[*it];		
	}
	for(auto it = clients.begin(); it != clients.end(); it++){
		int facility = this->assignment[*it];
		this->solution_value += this->ref_instance.dist_costs[facility][*it];
	}
}

std::vector<int> Validator::get_assignment(){
	return this->assignment;
}

float Validator::value(){
	return this->solution_value;
}

bool Validator::feasible(){
	if(this->open_facilities.size() == 0){ return false; }
	return this->violating_facilities().empty();
}

std::vector<int> Validator::violating_facilities(){
	std::vector<int> violating_facilities = std::vector<int>();
	std::vector<int> clients = range(this->ref_instance.clients);
	std::vector<float> capacities = this->ref_instance.capacities;
	// Reduce each "capacity" by the assigned demands. 
	for(auto it = clients.begin(); it != clients.end(); it++){
		int facility = this->assignment[*it];
		capacities[facility] -= this->ref_instance.demands[*it];
		if(!contains(violating_facilities, facility)){
			violating_facilities.push_back(facility);
		}
	}
	std::function<bool(int)> predicate = [capacities](int facility){ return capacities[facility] < 0; };
	filter(violating_facilities, predicate);
	return violating_facilities;
}