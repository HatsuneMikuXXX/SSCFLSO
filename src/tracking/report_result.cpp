#include "report_result.h"

ReportResult::ReportResult() : instance({}) {
	finishedUp = true;
}

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
	finishedUp = false;
}

ReportResult ReportResult::dummy_report() {
	static ReportResult dummy = ReportResult();
	return dummy;
}

void ReportResult::evalResult(const solution_and_value& current_best, Timer& timer) {
	if (finishedUp) { return; }
	LastSolution = current_best;
	time_stamps.push_back(timer.get_elapsed_time());
	value_stamps.push_back(LastSolution.val);
	number_of_facilities_stamps.push_back(asa::sum(LastSolution.sol, 0)); // This works even of type vector<bool>
}

void ReportResult::finishUp(const std::string& save_to_path) {
	if (finishedUp) { return; }
	finishedUp = true;
	// Compute feasibility
	Validator FLV = Validator(instance);
	FLV.set_solution(LastSolution.sol);
	LastSolutionFeasible = FLV.feasible();
	// Time stamp
	std::time_t tmp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string time(std::ctime(&tmp));
	time.erase(time.end() - 1, time.end()); // Remove line break
	std::replace(time.begin(), time.end(), ':', '-');
	std::replace(time.begin(), time.end(), ' ', '_');
	// Write results
	std::string instance_name_without_end = instance_name;
	instance_name_without_end.erase(instance_name_without_end.find(".plc"), 4);
	std::ofstream out(save_to_path + "/" + instance_name_without_end + "_" + algorithm_name + "_" + time + ".json");
	out << "{\"Instance ID\" : \"" << instance_name << "\", ";
	out << "\"Time limit (ms)\" : \"" << timelimit << "\", ";
	out << "\"Algorithm\" : \"" << algorithm_name << "\", ";
	out << "\"Gurobi Postprocessing\" : \"" << gurobi_postprocessing << "\", ";
	out << "\"Feasible\" : " << LastSolutionFeasible << ", ";
	out << "\"Solution Value\" : " << LastSolution.val << ", ";
	out << "\"Solution\" : " << primitive_list_to_string(LastSolution.sol) << ", ";
	out << "\"Time stamps\" : " << primitive_list_to_string(time_stamps) << ", ";
	out << "\"Value stamps\" : " << primitive_list_to_string(value_stamps) << ", ";
	out << "\"Number of open facilities stamps\" : " << primitive_list_to_string(number_of_facilities_stamps) << "} ";
	out.close();
}