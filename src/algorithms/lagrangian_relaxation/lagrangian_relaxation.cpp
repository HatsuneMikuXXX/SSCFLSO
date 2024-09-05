#include "lagrangian_relaxation.h"

std::string LagrangianRelaxation::name() const {
	return "Lagrangian Relaxation";
}

bool LagrangianRelaxation::post_applyable() const {
	return false;
}

void LagrangianRelaxation::solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const {
	Validator FLV(instance);
	facility_vector solution(instance.facilities);

	// Find an upper bound
	LocalSearch ls = LocalSearch(LocalSearch::PREPROCESS, LocalSearch::FIRST);
	ls.solve(instance, current_best, timer, report, false);
	FLV.set_solution(current_best.sol);
	if (!FLV.feasible()) {
		// Infeasible
		return;
	}
	double UB = current_best.val;

	// Prepare
	range_vector facility_range = range(instance.facilities);
	range_vector client_range = range(instance.clients);

	// Get Model
	GRBModel model = constructLRModel(instance);
	// Initialize weights
	Weights weights;
	weights.demand_weights.resize(instance.clients, 0);
	weights.capacity_weights.resize(instance.facilities, 0);
	weights.preference_weights = std::vector<std::vector<double>>(instance.clients, std::vector<double>(instance.facilities, 0));
	update_weights_of_model(instance, facility_range, client_range, model, weights);

	// Parameters
	double beta = 2;

	// Helper
	double LR_value = DBL_MAX;
	double no_improvements_counter = 0;

	// Apply LR
	do {
		// Solve
		model.optimize();
		if (model.get(GRB_IntAttr_Status) != GRB_OPTIMAL) {
			// Parameterized model not solved to optimality
			throw std::runtime_error("LR not solved to optimality. Maybe infeasible?");
			return;
		}

		if (get_gradient_magnitude(facility_range, client_range, instance, model) < 10e-6) {
			// Copy solution
			asa::for_each(facility_range, [&solution, &model](const int facility_id) {
				solution[facility_id] = bool(model.getVarByName(std::to_string(facility_id)).get(GRB_DoubleAttr_X));
			});
			break;
		}
		
		// Update parameters if necessary
		no_improvements_counter = (model.get(GRB_DoubleAttr_ObjVal) < LR_value) ? 0 : no_improvements_counter++;
		LR_value = model.get(GRB_DoubleAttr_ObjVal);
		if (no_improvements_counter >= 3) {
			beta /= 2;
			no_improvements_counter = 0;
		}

		// Update weights
		update_weights(facility_range, client_range, weights, instance, beta, UB, model);
		update_weights_of_model(instance, facility_range, client_range, model, weights);
		
	} while (true);

	FLV.set_solution(solution);
	if (!FLV.feasible() && !attempt_to_find_feasible_solution(solution, FLV)) {
		return;
	}
	improve_solution(instance, current_best, solution, timer, report);
	if (gurobi_afterwards) { solve_with_gurobi_afterwards(instance, current_best, solution, timer, report); }
}

double LagrangianRelaxation::get_gradient_magnitude(const range_vector& facility_range, const range_vector& client_range, const SSCFLSO& instance, GRBModel& model) const {
	double gradient_magnitude = 0;
	asa::for_each(client_range, [&gradient_magnitude, &facility_range, &model](const int client_id) {
		double gradient_part = -1;
		asa::for_each(facility_range, [&gradient_part, &model, &client_id](const int facility_id) {
			gradient_part += model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).get(GRB_DoubleAttr_X);
			});
		gradient_magnitude += pow(gradient_part, 2);
		});

	asa::for_each(facility_range, [&gradient_magnitude, &client_range, &instance, &model](const int facility_id) {
		double gradient_part = instance.capacities[facility_id];
		asa::for_each(client_range, [&gradient_part, &instance, &facility_id, &model](const int client_id) {
			gradient_part += instance.demands[client_id] * model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).get(GRB_DoubleAttr_X);
			});
		gradient_magnitude += pow(gradient_part, 2);
		});

	asa::for_each(client_range, [&gradient_magnitude, &facility_range, &instance, &model](const int client_id) {
		client_preference_vector rankings;
		inverse(rankings, instance.preferences[client_id]);
		asa::for_each(facility_range, [&model, &client_id, &facility_range, &rankings, &gradient_magnitude](const int facility_id) {
			double gradient_part = -model.getVarByName(std::to_string(facility_id)).get(GRB_DoubleAttr_X);
			asa::for_each(facility_range, [&gradient_part, &model, &client_id, &facility_id, &rankings](const int inner_facility_id) {
				if (rankings[inner_facility_id] <= rankings[facility_id]) {	// Reminder, low rank = high preference
					gradient_part += model.getVarByName(std::to_string(inner_facility_id) + ":" + std::to_string(client_id)).get(GRB_DoubleAttr_X);
				}
				});
			gradient_magnitude += pow(gradient_part, 2);
			});
		});
	return gradient_magnitude;
}


void LagrangianRelaxation::update_weights(const range_vector& facility_range, const range_vector& client_range, Weights& weights, const SSCFLSO& instance, double beta, double UB, GRBModel& model) const {
	// ### Compute the gradient parts, each loop represents a relaxed constraint ###
	// Alpha scalar value
	double gradient_magnitude = get_gradient_magnitude(facility_range,client_range, instance, model);
	double alpha = (gradient_magnitude > 0) ? beta * (model.get(GRB_DoubleAttr_ObjVal) - UB) / gradient_magnitude : 0;

	// Now update weights
	asa::for_each(client_range, [&weights, &alpha, &facility_range, &model](const int client_id) {
		double gradient_part = -1;
		asa::for_each(facility_range, [&gradient_part, &model, &client_id](const int facility_id) {
			gradient_part += model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).get(GRB_DoubleAttr_X);
		});
		weights.demand_weights[client_id] -= (alpha * gradient_part > 0) ? alpha * gradient_part : 0;
	});

	asa::for_each(facility_range, [&weights, &alpha, &client_range, &instance, &model](const int facility_id) {
		double gradient_part = instance.capacities[facility_id];
		asa::for_each(client_range, [&gradient_part, &instance, &facility_id, &model](const int client_id) {
			gradient_part += instance.demands[client_id] * model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).get(GRB_DoubleAttr_X);
			});
		weights.capacity_weights[facility_id] -= (alpha * gradient_part > 0) ? alpha * gradient_part : 0;
	});

	asa::for_each(client_range, [&weights, &alpha, &facility_range, &instance, &model](const int client_id) {
		client_preference_vector rankings;
		inverse(rankings, instance.preferences[client_id]);
		asa::for_each(facility_range, [&model, &client_id, &facility_range, &rankings, &weights, &alpha](const int facility_id) {
			double gradient_part = -model.getVarByName(std::to_string(facility_id)).get(GRB_DoubleAttr_X);
			asa::for_each(facility_range, [&gradient_part, &model, &client_id, &facility_id, &rankings](const int inner_facility_id) {
				if (rankings[inner_facility_id] <= rankings[facility_id]) {	// Reminder, low rank = high preference
					gradient_part += model.getVarByName(std::to_string(inner_facility_id) + ":" + std::to_string(client_id)).get(GRB_DoubleAttr_X);
				}
			});
			weights.preference_weights[client_id][facility_id] -= (alpha * gradient_part > 0) ? alpha * gradient_part : 0;
		});
	});
}

void LagrangianRelaxation::update_weights_of_model(const SSCFLSO& instance, const range_vector& facility_range, const range_vector& client_range, GRBModel& model, const Weights& weights) const {
	asa::for_each(facility_range, [&instance, &model, &client_range, &weights](const int facility_id) {
		// Coefficient of y variables (facility decision variable)
		double y_coeff = instance.facility_costs[facility_id];
		asa::for_each(client_range, [&y_coeff, &weights, &facility_id](const int client_id) {
			y_coeff += weights.preference_weights[client_id][facility_id];
		});
		model.getVarByName(std::to_string(facility_id)).set(GRB_DoubleAttr_Obj, y_coeff);
	});

	asa::for_each(facility_range, [&instance, &model, &client_range, &weights](const int facility_id) {
		asa::for_each(client_range, [&instance, &weights, &facility_id, &model](const int client_id) {
			// Coefficient of x variables (client assignment decision variable)
			double x_coeff = instance.distribution_costs[facility_id][client_id] - weights.demand_weights[client_id] + weights.capacity_weights[facility_id] * instance.demands[client_id] - weights.preference_weights[client_id][facility_id];
			model.getVarByName(std::to_string(facility_id) + ":" + std::to_string(client_id)).set(GRB_DoubleAttr_Obj, x_coeff);
		});
	});
	model.update();
}


GRBModel LagrangianRelaxation::constructLRModel(const SSCFLSO& instance) const {
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
		model.update();
		return model;
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	return NULL;
}

