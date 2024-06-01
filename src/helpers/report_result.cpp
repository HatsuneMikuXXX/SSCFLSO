#include "report_result.h"

ReportResult::ReportResult(const SSCFLSO& instance, const std::string& instance_name) {
	Validator tmp = Validator(instance);
	this->FLV = tmp;
	this->instance_name = instance_name;
	this->finishedUp = false;
	this->time_stamps = std::vector<double>();
	this->value_stamps = std::vector<double>();
	this->number_of_facilities_stamps = std::vector<int>();
}

void ReportResult::evalResult(const solution_and_value& current_best, Timer& timer) {
	if (this->finishedUp) { return; }
	this->LastSolution = current_best;
	this->time_stamps.push_back(timer.get_elapsed_time());
	this->value_stamps.push_back(this->LastSolution.val);
	this->number_of_facilities_stamps.push_back(sum(this->LastSolution.sol));
}

void ReportResult::finishUp(const std::string& save_to_path) {
	this->finishedUp = true;
	// Compute feasibility
	this->FLV.set_solution(this->LastSolution.sol);
	this->LastSolutionFeasible = FLV.feasible();
	// Write result
	std::ofstream out(save_to_path);
	out << "{\"Instance ID\" : \"" << this->instance_name << "\", ";
	out << "\"Time limit\" : \"" << this-> << "\", ";
	out << "\"Last solution (LS)\" : " << primitive_list_to_string(this->LastSolution.sol) << ", ";
	out << "\"LS value\" : " << this->LastSolution.val << ", ";
	out << "\"LS feasible\" : " << this->LastSolutionFeasible << ", ";
	out << "\"Time stamps\" : " << primitive_list_to_string(this->time_stamps) << ", ";
	out << "\"Value stamps\" : " << primitive_list_to_string(this->value_stamps) << ", ";
	out << "\"Number of open facilities stamps\" : " << primitive_list_to_string(this->number_of_facilities_stamps) << "} ";
	out.close();
}