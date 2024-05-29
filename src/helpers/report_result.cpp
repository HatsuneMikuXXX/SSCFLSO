#include "report_result.h"

ReportResult::ReportResult(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path) {
	this->instance = instance;
	this->instance_name = instance_name;
	this->save_to_path = save_to_path;
	this->finishedUp = false;
	this->time_stamps = std::vector<double>();
	this->value_stamps = std::vector<double>();
	this->number_of_facilities_stamps = std::vector<int>();
	this->AVG_rank_stamps = std::vector<double>();
	this->STD_rank_stamps = std::vector<double>();
}

void ReportResult::evalResult(const solution_and_value& current_best, Timer& timer) {
	if (finishedUp) { return; }
	// Update solution
	this->LastSolution = current_best;

	// Get other measures
	this->time_stamps.push_back(timer.get_elapsed_time());
	this->value_stamps.push_back(this->LastSolution.val);
	this->number_of_facilities_stamps.push_back(sum(this->LastSolution.sol));

	// Compute other measures
	Validator FLV = Validator(this->instance); // [TODO]: Find a way to not always initialize this over and over again.
	FLV.set_solution(this->LastSolution.sol);
	std::vector<double> ranks = std::vector<double>();
	double avg_rank = 0;
	{
		// AVG
		for (int i = 0; i < this->instance.clients; i++) {
			int assigned_facility = FLV.get_assignment()[i];
			int rank_of_assigned_facility = -1;
			for (int j = 0; j < this->instance.facilities; j++) {
				if (assigned_facility == this->instance.preferences[i][j]) {
					rank_of_assigned_facility = j + 1; // One-based
					break;
				}
			}
			assert(rank_of_assigned_facility != -1);
			ranks.push_back(rank_of_assigned_facility);
		}
		avg_rank = sum(ranks) / this->instance.clients;
		this->AVG_rank_stamps.push_back(avg_rank);
	}
	{
		// STD
		double std_rank = 0;
		double sum_squared_deviations = 0;
		for (auto it = ranks.begin(); it != ranks.end(); it++) {
			sum_squared_deviations += pow(*it - avg_rank, 2);
		}
		std_rank = sqrt(sum_squared_deviations / this->instance.clients);
		this->STD_rank_stamps.push_back(std_rank);
	}
}

void ReportResult::finishUp() {
	finishedUp = true;
	// Compute feasibility
	Validator FLV = Validator(this->instance);
	FLV.set_solution(this->LastSolution.sol);
	this->LastSolutionFeasible = FLV.feasible();
	// Write result
	std::ofstream out(this->save_to_path);
	out << "{\"Instance ID\" : \"" << this->instance_name << "\", ";
	out << "\"Last solution (LS)\" : " << primitive_list_to_string(this->LastSolution.sol) << ", ";
	out << "\"LS value\" : " << this->LastSolution.val << ", ";
	out << "\"LS feasible\" : " << this->LastSolutionFeasible << ", ";
	out << "\"Time stamps\" : " << primitive_list_to_string(this->time_stamps) << ", ";
	out << "\"Value stamps\" : " << primitive_list_to_string(this->value_stamps) << ", ";
	out << "\"Number of open facilities stamps\" : " << primitive_list_to_string(this->number_of_facilities_stamps) << ", ";
	out << "\"Averages of ranks\" : " << primitive_list_to_string(this->AVG_rank_stamps) << ", ";
	out << "\"Standard deviatiosn of ranks\" : " << primitive_list_to_string(this->STD_rank_stamps) << "}";
	out.close();
}