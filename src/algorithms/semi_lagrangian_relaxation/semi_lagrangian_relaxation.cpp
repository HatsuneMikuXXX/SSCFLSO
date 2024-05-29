#include "semi_lagrangian_relaxation.h"

void SemiLagrangianRelaxation::solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards) {
	auto start = start_timer();
	current_best = facility_vector(instance.facilities, 0);
	Validator FLV = Validator(instance);
	// Create weight update matrix
	std::vector<std::vector<double>> weights = weight_update_matrix(instance);
	std::vector<double> SLR_weights = std::vector<double>(instance.clients, 0);
	std::vector<double> final_weights = std::vector<double>(instance.clients, 0);
	std::vector<int> index_set = std::vector<int>(instance.clients, 0);
	for (int i = 0; i < instance.clients; i++) {
		SLR_weights[i] = weights[i][0];
		final_weights[i] = weights[i][weights[i].size() - 1];
	}

	// Create model and set weights
	GRBModel model = constructSLRModel(instance);
	for (int i = 0; i < instance.clients; i++) {
		for (int j = 0; j < instance.facilities; j++) {
			GRBVar x = model.getVarByName(std::to_string(j) + ":" + std::to_string(i));
			x.set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i] - SLR_weights[i]);
		}
	}
	// Repeatedly solve
	int iter = 0;
	int maxIter = 1000;
	bool start_hypothesis = false;
	facility_vector comparison;
	int no_change = 0;
	do {
		iter++;
		// Solve
		model.update();
		model.set(GRB_DoubleParam_TimeLimit, 600);
		model.optimize();
		facility_vector solution = facility_vector(instance.facilities, -1);
		for (int j = 0; j < instance.facilities; j++) {
			GRBVar x = model.getVarByName(std::to_string(j));
			solution[j] = x.get(GRB_DoubleAttr_X);
		}
		if (within_time_limit(start, time_limit)) {
			current_best = solution;
		}
		if (start_hypothesis) {
			if (areSame(solution, comparison)) {
				no_change += 1;
			}
		}
		else {
			start_hypothesis = true;
		}
		comparison = solution;
		std::vector<double> gradient = std::vector<double>(instance.clients, 1);
		for (int i = 0; i < instance.clients; i++) {
			for (int j = 0; j < instance.facilities; j++) {
				GRBVar x = model.getVarByName(std::to_string(j) + ":" + std::to_string(i));
				gradient[i] -= x.get(GRB_DoubleAttr_X);
			}
		}
		if (magnitude(gradient) == 0) {
			std::cout << "No improvements: " << no_change << std::endl;
			std::cout << "Total iterations: " << iter << std::endl;
			return;
		}
		else {
			// Update Weights
			for (int i = 0; i < instance.clients; i++) {
				if (gradient[i] > 0) {
					index_set[i] += (index_set[i] == weights[i].size() - 1) ? 0 : 1;
					SLR_weights[i] = weights[i][index_set[i]];
				}
			}
			for (int i = 0; i < instance.clients; i++) {
				for (int j = 0; j < instance.facilities; j++) {
					GRBVar x = model.getVarByName(std::to_string(j) + ":" + std::to_string(i));
					x.set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i] - SLR_weights[i]);
				}
			}
		}
	} while (iter < maxIter && within_time_limit(start, time_limit));
	std::cout << "No improvements: " << no_change << std::endl;
	std::cout << "Total iterations: " << iter << std::endl;
};

	

std::string SemiLagrangianRelaxation::meta_information(){
	std::ifstream file(PATH + "semi_lagrangian_relaxation/semi_lagrangian_relaxation.txt");
	if (file) {
		std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		return content;
	}
	else {
		throw std::runtime_error("Semi Lagrangian Relaxation information file not found.");
	}
}

std::vector<std::vector<double>> SemiLagrangianRelaxation::weight_update_matrix(const SSCFLSO& instance) {
	const std::function<bool(const double&, const double&)> is_less = [](double a, double b) { return a < b; };
	std::vector<double> sorted_facility_cost = {};
	for (int j = 0; j < instance.facilities; j++) {
		bisect_insert(sorted_facility_cost, instance.facility_costs[j], is_less);
	}
	std::vector<std::vector<double>> weights = {};
	for (int i = 0; i < instance.clients; i++) {
		weights.push_back(std::vector<double>());
		double max_distance = -1;
		for (int j = 0; j < instance.facilities; j++) {
			bisect_insert(weights[i], instance.distribution_costs[j][i] + 0.005, is_less);
			max_distance = (max_distance < instance.distribution_costs[j][i]) ? instance.distribution_costs[j][i] : max_distance;
		}
		double accumulated_weight = max_distance + 0.005;
		for (int j = 0; j < instance.facilities; j++) {
			accumulated_weight += sorted_facility_cost[j];
			bisect_insert(weights[i], accumulated_weight, is_less);
		}
		// Remove duplicates
		double tmp = -1; // Weights are non-negative - so we can use negative numbers
		auto it = weights[i].begin();
		while (it != weights[i].end()) {
			if (*it == tmp) {
				it = weights[i].erase(it);
				continue;
			}
			else {
				tmp = *it;
				it++;
			}
		}
	}
	return weights;
}

GRBModel SemiLagrangianRelaxation::constructSLRModel(const SSCFLSO& instance) {
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
			open[j].set(GRB_StringAttr_VarName, std::to_string(j));
		}
		// Distribution variables
		GRBVar** distribution = new GRBVar * [instance.facilities];
		for (int j = 0; j < m; j++) {
			distribution[j] = model.addVars(n, GRB_BINARY);
			for (int i = 0; i < n; i++) {
				distribution[j][i].set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i]);
				distribution[j][i].set(GRB_StringAttr_VarName, std::to_string(j) + ":" + std::to_string(i));
			}
		}
		// Objective
		model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
		// Constraint demand
		for (int i = 0; i < n; i++) {
			GRBLinExpr totdemand = 0;
			for (int j = 0; j < m; j++) {
				totdemand += distribution[j][i];
			}
			model.addConstr(totdemand <= 1);
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
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				GRBLinExpr preference = open[j];
				bool start_to_add = false;
				for (auto it = instance.preferences[i].begin(); it != instance.preferences[i].end(); it++) {
					if (start_to_add) {
						preference += distribution[*it][i];
					}
					start_to_add = (*it == j) ? true : start_to_add;
				}
				model.addConstr(preference <= 1);
			}
		}
		model.update();
		return model;
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
}


