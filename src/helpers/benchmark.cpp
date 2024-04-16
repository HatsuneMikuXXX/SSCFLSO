#include "benchmark.h"

void run(Algorithm* algo, const bool gurobi_afterwards, const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, int timelimit_in_milliseconds) {
	facility_vector solution = facility_vector(instance.facilities, -1);
	std::chrono::milliseconds timelimit_as_chrono(timelimit_in_milliseconds);
	Preprocess DoNothingObject = Preprocess(); // We do not preprocess anything, we just need access to Algorithm methods
	auto start = DoNothingObject.start_timer();
	algo->solve(instance, solution, timelimit_as_chrono, gurobi_afterwards);
	auto measured_time = DoNothingObject.get_elapsed_time_ms(start).count();
	Validator FLV = Validator(instance);
	FLV.set_solution(solution);
	/*
	std::string meta_info = algo->meta_information();
	{
		meta_info.erase(std::remove(meta_info.begin(), meta_info.end(), '\n'), meta_info.end());
	}
	*/
	std::ofstream out(save_to_path);
	out << "{\n\t";
	out << "\"Instance id\" : \"" << instance_name << "\",\n\t";
	//out << "\"Algorithm information\" : \""  << meta_info << "\",\n\t";
	out << "\"Gurobi used afterwards\" : " << gurobi_afterwards << ",\n\t";
	out << "\"Solution Feasibility\" : " << FLV.feasible() << ",\n\t";
	out << "\"Solution\" : " << primitive_list_to_string(solution) << ",\n\t";
	out << "\"Solution value\": " << FLV.value() << ",\n\t";
	out << "\"Timelimit in milliseconds\" : " << timelimit_in_milliseconds << ",\n\t";
	out << "\"Time in milliseconds\" : " << measured_time << "\n";
	out << "}";
	out.close();
}
