#ifndef REPORT_H
#define REPORT_H
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../helpers/timer.h"
#include <fstream> 

class ReportResult {
public:
	ReportResult(
		const SSCFLSO& instance,
		const std::string& instance_name,
		const int timelimit,
		const std::string algorithm_name,
		const bool gurobi_postprocessing);
	void evalResult(const solution_and_value& current_best, Timer& timer);
	void finishUp(const std::string& save_to_path);
	static ReportResult dummy_report();
private:
	ReportResult(); // For the dummy report
	// Control flow
	bool finishedUp{true};
	// Information about the instance
	const SSCFLSO& instance;
	const std::string& instance_name{""};
	const int timelimit{0};
	const std::string algorithm_name{""};
	const bool gurobi_postprocessing{false};
	// Information to store
	solution_and_value LastSolution;
	bool LastSolutionFeasible{false};
	std::vector<double> time_stamps{ std::vector<double>() };
	std::vector<double> value_stamps{ std::vector<double>() };
	std::vector<int> number_of_facilities_stamps{ std::vector<int>() };
};



#endif