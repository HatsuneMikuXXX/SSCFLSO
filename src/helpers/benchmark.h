#ifndef BENCHMARK_H
#define BENCHMARK_H
#include "../SSCFLSO/SSCFLSO_validator.h"
#include "../SSCFLSO/SSCFLSO_generator.h"
#include <chrono> // Measuring time
#include <iostream> // Show status in the terminal
typedef std::function<std::vector<int>(SSCFLSO&)> algorithm;
void run(const std::vector<std::pair<std::string, algorithm>>& algorithms, 
	const std::vector<std::string>& instances, 
	const std::string& save_location, 
	float max_run_time, 
	bool preferences_included = true, 
	int precision = 4);
#endif