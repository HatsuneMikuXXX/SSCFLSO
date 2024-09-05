#ifndef REPORT_H
#define REPORT_H
#include "../SSCFLSO_model/SSCFLSO_validator.h"
#include "../SSCFLSO_model/SSCFLSO_generator.h"
#include "../helpers/timer.h"
#include <fstream> 
#include <ctime>
#include <chrono>

class ReportResult {
public:
	ReportResult(
		const std::string& directory,
		const SSCFLSO& instance,
		const std::string& instance_name,
		const std::string algorithm_name,
		const bool gurobi_postprocessing);
	void evalResult(const solution_and_value& current_best, const bool is_feasible, Timer& timer);
	static ReportResult dummy_report();
private:
	ReportResult(); // For the dummy report
	// Control flow
	// Information about the instance
	const SSCFLSO& instance;
	std::string file;
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