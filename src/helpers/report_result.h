#ifndef REPORT_H
#define REPORT_H
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../typedefinitions.h"
#include "../helpers/timer.h"

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
private:
	// Control flow
	bool finishedUp{false};
	// Information about the instance
	const SSCFLSO& instance;
	const std::string& instance_name;
	const int timelimit;
	const std::string algorithm_name;
	const bool gurobi_postprocessing;
	// Information to store
	solution_and_value LastSolution;
	bool LastSolutionFeasible;
	std::vector<double> time_stamps{ std::vector<double>() };
	std::vector<double> value_stamps{ std::vector<double>() };
	std::vector<int> number_of_facilities_stamps{ std::vector<int>() };
};

#endif