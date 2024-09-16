#include "report_result.h"

ReportResult::ReportResult() : instance{} {
}

ReportResult::ReportResult(
	const std::string& directory,
	const SSCFLSO& instance, 
	const std::string& instance_name, 
	const std::string algorithm_name, 
	const bool gurobi_postprocessing) :
	instance(instance), 
	instance_name(instance_name), 
	algorithm_name(algorithm_name), 
	gurobi_postprocessing(gurobi_postprocessing) {
		// Time stamp
		std::time_t tmp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string time(std::ctime(&tmp));
		time.erase(time.end() - 1, time.end()); // Remove line break
		std::replace(time.begin(), time.end(), ':', '-');
		std::replace(time.begin(), time.end(), ' ', '_');

		// Write results
		std::string instance_name_without_end = instance_name;
		instance_name_without_end.erase(instance_name_without_end.find(".plc"), 4);
		file = directory + "/" + instance_name_without_end + "_" + algorithm_name + "_" + time;
}

ReportResult ReportResult::dummy_report() {
	static ReportResult dummy = ReportResult();
	return dummy;
}

void ReportResult::evalResult(const solution_and_value& current_best, const bool is_feasible, Timer& timer) {
	LastSolution = current_best;
	time_stamps.push_back(timer.get_accumulated_cpu_time_in_ms());
	value_stamps.push_back(LastSolution.val);
	number_of_facilities_stamps.push_back(asa::sum(LastSolution.sol, 0));
	
	std::string b = is_feasible ? "true" : "false";

	std::string information = 
		"{\"Instance ID\" : \"" + instance_name + "\", " 
		+ "\"Algorithm\" : \"" + algorithm_name + "\", "
		+ "\"Feasible\" : " + b + ", "
		+ "\"Solution Value\" : " + std::to_string(LastSolution.val) + ", "
		+ "\"Solution\" : " + primitive_list_to_string(LastSolution.sol) + ", "
		+ "\"Time stamps\" : " + primitive_list_to_string(time_stamps) + ", "
		+ "\"Value stamps\" : " + primitive_list_to_string(value_stamps) + ", "
		+ "\"Number of open facilities stamps\" : " + primitive_list_to_string(number_of_facilities_stamps) + "} ";

	std::cout << information << std::endl;

	{
		std::ofstream out(file + ".json");
		out << information;
		out.close();
	}

	// Create safety-copy
	{
		std::ofstream out(file + "_safetycopy.json");
		out << information;
		out.close();
	}
}
