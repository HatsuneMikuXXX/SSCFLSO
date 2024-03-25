#ifndef BENCHMARK_H
#define BENCHMARK_H
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../SSCFLSO/SSCFLSO_generator.h"
#include "../common.h"
#include "../algorithms/DoNothing/DoNothing.h"
#include "../algorithms/algorithmClass.h"
typedef std::function<void(const SSCFLSO&, facility_vector&, const std::chrono::milliseconds&)> algorithm;
void run(Algorithm& algo, const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, int timelimit_in_milliseconds);
void initial_solution(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path);
void gaps(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, const facility_vector& initial_solution, double timelimit_in_seconds);
/*
void run(const std::vector<std::pair<std::string, algorithm>>& algorithms, 
	const std::vector<std::string>& instances, 
	const std::string& save_location, 
	float time_limit_MS, 
	bool preferences_included = true, 
	int precision = 4);*/


// To Retrieve
// Each algorithm, each instance: TIME, VALUE, Solution
// To compute
// Gap, Hamming Distance, Number of facilities
#endif