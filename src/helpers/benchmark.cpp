#include "benchmark.h"

void run(Algorithm& algo, const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, int timelimit_in_milliseconds) {
	facility_vector solution = facility_vector(instance.facilities, -1);
	std::chrono::milliseconds timelimit_as_chrono(timelimit_in_milliseconds);
	DoNothing DoNothingObject = DoNothing();
	auto start = DoNothingObject.start_timer();
	algo.solve(instance, solution, timelimit_as_chrono);
	auto measured_time = DoNothingObject.get_elapsed_time_ms(start).count();
	Validator FLV = Validator(instance);
	FLV.set_solution(solution);
	std::ofstream out(save_to_path);
	out << instance_name << "\n\n";
	out << algo.meta_information() << "\n\n";
	out << "Solution value: " << FLV.value() << "\n\n";
	out << "Timelimit in milliseconds: " << timelimit_in_milliseconds << "\n\n";
	out << "Time in milliseconds: " << measured_time << "\n\n";
	out << primitive_list_to_string(solution);
	out.close();
}

void initial_solution(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path) {
	DoNothing DoNothingObject = DoNothing();
	facility_vector solution = DoNothingObject.preprocess(instance);
	std::ofstream out(save_to_path);
	Validator FLV = Validator(instance);
	FLV.set_solution(solution);
	out << instance_name << "\n\n";
	out << "Solution value: " << FLV.value() << "\n\n";
	out << primitive_list_to_string(solution);
	out.close();
}

void gaps(const SSCFLSO& instance, const std::string& instance_name, const std::string& save_to_path, const facility_vector& initial_solution, double timelimit_in_seconds) {
	DoNothing DoNothingObject = DoNothing();
	auto start = DoNothingObject.start_timer();
	
	int m = instance.facilities;
	int n = instance.clients;
	try {
		// Define the model
		GRBEnv* env = new GRBEnv();
		env->set(GRB_IntParam_OutputFlag, 0); // Surpress Console
		GRBModel model = GRBModel(*env);
		// Facility variables
		GRBVar* open = model.addVars(m, GRB_BINARY);
		for (int j = 0; j < m; j++) {
			open[j].set(GRB_DoubleAttr_Obj, instance.facility_costs[j]);
		}
		// Distribution variables
		GRBVar** distribution = new GRBVar * [instance.facilities];
		for (int j = 0; j < m; j++) {
			distribution[j] = model.addVars(n, GRB_BINARY);
			for (int i = 0; i < n; i++) {
				distribution[j][i].set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i]);
			}
		}
		// Objective - Constant coefficient are irrelevant
		model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
		// Constraint demand
		for (int i = 0; i < n; i++) {
			GRBLinExpr totdemand = 0;
			for (int j = 0; j < m; j++) {
				totdemand += distribution[j][i];
			}
			model.addConstr(totdemand == 1);
		}
		// Constraint capacity
		for (int j = 0; j < m; j++) {
			GRBLinExpr totdemand = 0;
			for (int i = 0; i < n; i++) {
				totdemand += instance.demands[i] * distribution[j][i];
			}
			model.addConstr(totdemand <= instance.capacities[j] * open[j]);
		}
		// Constraint preference
		std::vector<std::vector<int>> rankings = std::vector<std::vector<int>>(n);
		for (int i = 0; i < n; i++) {
			rankings[i] = inverse(instance.preferences[i]);
			for (int j = 0; j < m; j++) {
				GRBLinExpr preference = open[j];
				for (int k = 0; k < m; k++) {
					if (rankings[i][k] > rankings[i][j]) {
						preference += distribution[k][i];
					}
				}
				model.addConstr(preference <= 1);
			}
		}

		// Initial solution
		//for (int j = 0; j < m; j++) {
			//open[j].set(GRB_DoubleAttr_Start, initial_solution[j]);
		//}
		auto subtract = DoNothingObject.get_elapsed_time_ms(start).count();
		model.set(GRB_DoubleParam_TimeLimit, (timelimit_in_seconds - subtract)/1000);
		model.optimize();
		auto measured_time = DoNothingObject.get_elapsed_time_ms(start).count();
		if (model.get(GRB_IntAttr_Status) == 3) {
			std::ofstream out(save_to_path);
			out << instance_name << "\n\nInfeasible";
			out.close();
			return;
		}
		facility_vector solution = facility_vector(instance.facilities, -1);
		for (int j = 0; j < m; j++) {
			solution[j] = open[j].get(GRB_DoubleAttr_X);
		}

		Validator FLV = Validator(instance);
		FLV.set_solution(solution);
		std::ofstream out(save_to_path);
		out << instance_name << "\n\n";
		out << "Time limit in seconds: " << timelimit_in_seconds << "\n\n";
		out << "Time: " << measured_time << "\n\n";
		out << "Solution value: " << FLV.value() << "\n\n";
		out << "Feasible: " << FLV.feasible() << "\n\n";
		out << primitive_list_to_string(solution);
		out.close();
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	catch (GRBException e) {
		std::cerr << "Something went wrong:\n" << e.getMessage() << std::endl;
	}
}
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