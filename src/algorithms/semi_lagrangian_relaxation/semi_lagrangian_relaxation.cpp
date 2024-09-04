#include "semi_lagrangian_relaxation.h"

SemiLagrangianRelaxation::SemiLagrangianRelaxation(bool homogenous_weight_update) : homogenous_weight_update(homogenous_weight_update){}

std::string SemiLagrangianRelaxation::name() const {
	
	return (homogenous_weight_update) ? "Semi-Lagrangian Relaxation homogenous_weights" : "Semi-Lagrangian Relaxation ";
}

bool SemiLagrangianRelaxation::post_applyable() const {
	return false;
}

void SemiLagrangianRelaxation::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	Validator FLV(instance);
	facility_vector solution(instance.facilities);
	
	// Check if a feasible solution exists
	Preprocess p = Preprocess();
	p.solve(instance, current_best, timer, report, false);
	FLV.set_solution(current_best.sol);
	if (!FLV.feasible()) {
		return;
	}
	
	range_vector facility_range = range(instance.facilities);
	range_vector client_range = range(instance.clients);
	
	// Create weight update matrix
	std::vector<std::vector<double>> weights = weight_update_matrix(instance);
	std::vector<double> SLR_weights(instance.clients, 0);
	std::vector<double> final_weights(instance.clients, 0);
	std::vector<int> index_set(instance.clients, 0);
	
	asa::for_each(client_range, [&SLR_weights, &final_weights, &weights](const int client_id) {
		SLR_weights[client_id] = weights[client_id][0];
		final_weights[client_id] = weights[client_id].back();
	});

	// Create model and set weights
	GRBModel model = constructSLRModel(instance);
	asa::for_each(client_range, [&model, &instance, &SLR_weights, &facility_range](const int client_id) {
		asa::for_each(facility_range, [&model, &instance, &SLR_weights, &client_id](const int facility_id) {
			model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).set(GRB_DoubleAttr_Obj, instance.distribution_costs[facility_id][client_id] - SLR_weights[client_id]);
		});
	});

	// Repeatedly solve
	do {
		// Solve
		model.update();
		model.set(GRB_DoubleParam_TimeLimit);
		model.optimize();

		if (model.get(GRB_IntAttr_Status) != GRB_OPTIMAL) {
			// Parameterized model not solved to optimality
			return;
		}

		// Copy solution
		asa::for_each(facility_range, [&solution, &model](const int facility_id) {
			solution[facility_id] = bool(model.getVarByName(std::to_string(facility_id)).get(GRB_DoubleAttr_X));
		});
		
		// Compute gradient
		std::vector<double> gradient(instance.clients, 1);
		asa::for_each(client_range, [&facility_range, &model, &gradient](const int client_id) {
			asa::for_each(facility_range, [&client_id, &model, &gradient](const int facility_id) {
				gradient[client_id] -= model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).get(GRB_DoubleAttr_X);
			});
		});

		if (magnitude(gradient) <= 10e-8) {
			// Optimal solution found
			improve_solution(instance, current_best, solution, timer, report);
			return;
		}
		else {
			// Update Weights
			asa::for_each(client_range, [this, &gradient, &index_set, &SLR_weights, &weights](const int client_id) {
				if (homogenous_weight_update || gradient[client_id] > 0) {
					index_set[client_id] += (index_set[client_id] == weights[client_id].size() - 1) ? 0 : 1;
					SLR_weights[client_id] = weights[client_id][index_set[client_id]];
				}
			});
			// Prepare model
			asa::for_each(client_range, [&instance, &facility_range, &model, &SLR_weights](const int client_id) {
				asa::for_each(facility_range, [&instance, &client_id, &model, &SLR_weights](const int facility_id) {
					model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).set(GRB_DoubleAttr_Obj, instance.distribution_costs[facility_id][client_id] - SLR_weights[client_id]);
				});
			});
		}
	} while (true);
};

std::vector<std::vector<double>> SemiLagrangianRelaxation::weight_update_matrix(const SSCFLSO& instance) const {
	range_vector facility_range = range(instance.facilities);
	range_vector client_range = range(instance.clients);
	
	facility_cost_vector sorted_facility_cost = instance.facility_costs;
	asa::sort(sorted_facility_cost, [](const double a, const double b) -> bool { return a < b; });

	std::vector<std::vector<double>> weights(0);
	asa::for_each(client_range, [&sorted_facility_cost, &weights, &facility_range, &instance](const int client_id) {
		weights.push_back(std::vector<double>(0));
		double max_distance = -1;
		asa::for_each(facility_range, [&instance, &client_id, &max_distance, &weights](const int facility_id) {
			weights[client_id].push_back(instance.distribution_costs[facility_id][client_id] + 0.005);
			max_distance = (max_distance < instance.distribution_costs[facility_id][client_id]) ? instance.distribution_costs[facility_id][client_id] : max_distance;
		});

		double accumulated_weight = max_distance + 0.005;
		asa::for_each(facility_range, [&client_id, &accumulated_weight, &sorted_facility_cost, &weights](const int facility_id) {
			accumulated_weight += sorted_facility_cost[facility_id];
			weights[client_id].push_back(accumulated_weight);
		});
		asa::sort(weights[client_id], [](const double a, const double b) -> bool { return a < b; });
		weights[client_id].erase(std::unique(std::begin(weights[client_id]), std::end(weights[client_id])), std::end(weights[client_id]));
	});
	return weights;
}

GRBModel SemiLagrangianRelaxation::constructSLRModel(const SSCFLSO& instance) const {
	int m = instance.facilities;
	int n = instance.clients;
	range_vector facility_range = range(m);
	range_vector client_range = range(n);
	try {
		// Define the model
		GRBEnv* env = new GRBEnv();
		env->set(GRB_IntParam_OutputFlag, 0); // Surpress Console
		GRBModel model = GRBModel(*env);
		// Facility variables
		GRBVar* open = model.addVars(m, GRB_BINARY);
		asa::for_each(facility_range, [&open, &instance](const int facility_id) {
			open[facility_id].set(GRB_DoubleAttr_Obj, instance.facility_costs[facility_id]);
			open[facility_id].set(GRB_StringAttr_VarName, std::to_string(facility_id));
		});

		// Distribution variables
		GRBVar** distribution = new GRBVar * [instance.facilities];
		asa::for_each(facility_range, [&distribution, &instance, &n, &model, &client_range](const int facility_id) {
			distribution[facility_id] = model.addVars(n, GRB_BINARY);
			asa::for_each(client_range, [&distribution, &instance, &facility_id](const int client_id) {
				distribution[facility_id][client_id].set(GRB_StringAttr_VarName, std::to_string(facility_id) + ":" + std::to_string(client_id));
			});
		});

		// Objective
		model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

		// Constraint demand
		asa::for_each(client_range, [&facility_range, &distribution, &model](const int client_id) {
			GRBLinExpr totdemand = 0;
			asa::for_each(facility_range, [&totdemand, &distribution, &client_id](const int facility_id) {
				totdemand += distribution[facility_id][client_id];
			});
			model.addConstr(totdemand <= 1);
		});

		// Constraint capacity
		asa::for_each(facility_range, [&instance, &distribution, &model, &open, &client_range](const int facility_id) {
			GRBLinExpr totdemand = 0;
			asa::for_each(client_range, [&totdemand, &instance, &distribution, &facility_id](const int client_id) {
				totdemand += instance.demands[client_id] * distribution[facility_id][client_id];
				});
			model.addConstr(totdemand <= instance.capacities[facility_id] * open[facility_id]);
			});

		// Constraint preference
		asa::for_each(client_range, [&model, &facility_range, &open, &distribution, &instance](const int client_id) {
			std::vector<int> rankings_of_client(0); // Low Rank-Value = High Preference
			inverse(rankings_of_client, instance.preferences[client_id]);
			asa::for_each(facility_range, [&model, &client_id, &open, &facility_range, &distribution, &rankings_of_client](const int facility_id) {
				GRBLinExpr preference = open[facility_id];
				asa::for_each(facility_range, [&client_id, &facility_id, &preference, &distribution, &rankings_of_client](const int worse_facility_id) {
					if (rankings_of_client[worse_facility_id] > rankings_of_client[facility_id]) {
						preference += distribution[worse_facility_id][client_id];
					}
					});
				model.addConstr(preference <= 1);
				});
		});
		model.update();
		return model;
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	return NULL;
}


