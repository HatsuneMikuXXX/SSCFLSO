#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../common.h"
#include "../algorithms/algorithmClass.h"
#include "../SSCFLSO_model/SSCFLSO_validator.h"

void run(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, int timelimit_in_milliseconds, Algorithm* const algo_ptr, const bool gurobi_afterwards);


/*
void run(const std::vector<std::pair<std::string, algorithm>>& algorithms, 
	const std::vector<std::string>& instances, 
	const std::string& save_location, 
	float time_limit_MS, 
	bool preferences_included = true, 
	int precision = 4);*/


// To Retrieve
// Each algorithm, each instance: TIME, VALUE, Solution, Number of Facilities, AVG Rank, AVG Rank Variance
// To compute
// Gap, Hamming Distance, Number of facilities
#endif