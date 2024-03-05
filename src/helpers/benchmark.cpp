#include "benchmark.h"
/*
void run(const std::vector<std::pair<std::string, algorithm>>& algorithms, 
	
	const std::vector<std::string>& instances, 
	const std::string& save_location, 
	float max_run_time, 
	bool preferences_included, 
	int precision)
	{/*
	try{
	std::vector<int> solution;
	float solution_value;
	float runtime;
	std::ofstream out(save_location);
	for(std::pair<std::string, algorithm> algo : algorithms){
		std::cout << "Running " + algo.first << std::endl;
		for(std::string path : instances){
			std::cout << "Loading " + path << std::endl;
			SSCFLSO instance = Generator::load_instance(path, preferences_included);
			Validator FLV = Validator(instance);
			std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
			solution = algo.second(instance);
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			FLV.set_solution(solution);
			solution_value = ceil(FLV.value() * float(pow(10, precision))) / float(pow(10, precision));
			runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			runtime = runtime / 1000.0;
			out << algo.first + " " + std::to_string(solution_value) + " " + std::to_string(runtime) + "s " + solution_to_string(solution) << std::endl;
		}
		out << "-------------------------------------------------------------------------------------------------------------------" << std::endl;
	}
	out.close();
	}
	catch(const std::runtime_error& e){
		std::cerr << e.what() << std::endl;
	}
}*/