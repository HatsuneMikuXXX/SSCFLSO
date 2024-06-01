#ifndef REPORT_H
#define REPORT_H
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../typedefinitions.h"
#include "../helpers/timer.h"

class ReportResult {
public:
	ReportResult(const SSCFLSO& instance, const std::string& instance_name);
	void evalResult(const solution_and_value& current_best, Timer& timer);
	void finishUp(const std::string& save_to_path);
private:
	// Control flow
	bool finishedUp;
	// Information about the instance
	Validator FLV;
	std::string instance_name;
	// Information to store
	solution_and_value LastSolution;
	bool LastSolutionFeasible;
	std::vector<double> time_stamps;
	std::vector<double> value_stamps;
	std::vector<int> number_of_facilities_stamps;
};

#endif