#include "benchmark.h"

void run(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, int timelimit_in_milliseconds, Algorithm* algo, const bool gurobi_afterwards) {
	ReportResult report(instance, instance_name);
	solution_and_value SV{ facility_vector(instance.facilities, 0) , -1};
	Timer timer(timelimit_in_milliseconds);
	algo->solve(instance, SV, timer, report, gurobi_afterwards);
	report.finishUp(save_to_path);
	
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
