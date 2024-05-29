#ifndef REPORT_H
#define REPORT_H
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../typedefinitions.h"
#include "../helpers/timer.h"

class ReportResult {
public:
	ReportResult(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path);
	void evalResult(const solution_and_value& current_best, Timer& timer);
	void finishUp();
private:
	// Control flow
	bool finishedUp;
	// Information about the instance
	std::string save_to_path;
	SSCFLSO instance;
	std::string instance_name;
	// Information to store
	solution_and_value LastSolution;
	bool LastSolutionFeasible;
	std::vector<double> time_stamps;
	std::vector<double> value_stamps;
	std::vector<int> number_of_facilities_stamps;
	std::vector<double> AVG_rank_stamps; // Average of ranks: Each client assigns a rank to a facility, compute the rank of the assigned facility
	std::vector<double> STD_rank_stamps; // Standard deviation of the AVG of ranks
};

#endif