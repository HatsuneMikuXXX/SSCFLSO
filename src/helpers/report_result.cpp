#include "report_result.h"

ReportResult::ReportResult(
	const SSCFLSO& instance, 
	const std::string& instance_name, 
	const int timelimit, 
	const std::string algorithm_name, 
	const bool gurobi_postprocessing) :
	instance(instance), 
	instance_name(instance_name), 
	timelimit(timelimit), 
	algorithm_name(algorithm_name), 
	gurobi_postprocessing(gurobi_postprocessing) {
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
	Validator FLV = Validator(this->instance);
	FLV.set_solution(this->LastSolution.sol);
	this->LastSolutionFeasible = FLV.feasible();
	// Write result
	std::ofstream out(save_to_path);
	out << "{\"Instance ID\" : \"" << this->instance_name << "\", ";
	out << "\"Time limit\" : \"" << this->timelimit << "\", ";
	out << "\"Algorithm\" : \"" << this->algorithm_name << "\",\n\t";
	out << "\"Gurobi Postprocessing\" : \"" << this->gurobi_postprocessing << "\",\n\t";
	out << "\"Feasible\" : " << this->LastSolutionFeasible << ", ";
	out << "\"Solution Value\" : " << this->LastSolution.val << ", ";
	out << "\"Solution\" : " << primitive_list_to_string(this->LastSolution.sol) << ", ";
	out << "\"Time stamps\" : " << primitive_list_to_string(this->time_stamps) << ", ";
	out << "\"Value stamps\" : " << primitive_list_to_string(this->value_stamps) << ", ";
	out << "\"Number of open facilities stamps\" : " << primitive_list_to_string(this->number_of_facilities_stamps) << "} ";
	out.close();
}