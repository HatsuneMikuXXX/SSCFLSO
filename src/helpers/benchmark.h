#ifndef BENCHMARK_H
#define BENCHMARK_H
#include "../SSCFLSO/SSCFLSO.h"
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../SSCFLSO/SSCFLSO_generator.h"
#include "../common.h"
#include "../helpers/timer.h"
#include "../algorithms/Preprocess/Preprocess.h"
#include "../algorithms/algorithmClass.h"


typedef std::function<void(const SSCFLSO&, facility_vector&, const std::chrono::milliseconds&)> algorithm;
void run(Algorithm* algo, const bool gurobi_afterwards, const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, int timelimit_in_milliseconds);


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