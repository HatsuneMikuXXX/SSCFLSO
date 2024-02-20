#include "semi_lagrangian_relaxation.h"
typedef std::vector<std::pair<int, int>> client_facility_edge_set;

facility_vector solve(const SSCFLSO& instance) {
	Validator FLV = Validator(instance);
	facility_vector no_unnecessary_facilities = preprocess(instance);
	if (sum(no_unnecessary_facilities) == 0) {
		return no_unnecessary_facilities;
	}
	FLV.set_solution(no_unnecessary_facilities);
	FLV.drop_empty_facilities();
	lagrange_multipliers initial = SGM(instance, FLV.value()); // Solve LR to get initial weights
	SLR_solution_and_subgradient intermediate = DAM(instance, initial); // Use DAM and repeatedly solve SLR
	FLV.set_solution(intermediate.solution);
	return FLV.feasible() ? intermediate.solution : no_unnecessary_facilities; // No guarantee that solution is feasible
}

SLR_solution_and_subgradient DAM(const SSCFLSO& instance, const lagrange_multipliers& initial) {
	SLR_solution_and_subgradient res;
	
	// Parameters
	int maxIter = 50;
	double epsilon = 5 * 10e-7;
	
	// Construct possible weight values
	std::vector<double> final_weight = std::vector<double>(instance.clients, 0);
	for (int i = 0; i < instance.clients; i++) {
		for (int j = 0; j < instance.facilities; j++) {
			double tmp = instance.facility_costs[j] + instance.distribution_costs[j][i];
			final_weight[i] = (tmp > final_weight[i]) ? tmp : final_weight[i];
		}
	}
	std::vector<std::vector<double>> weights = {};
	std::function<bool(double, double)> is_less = [](double a, double b) { return a < b; };
	for (int i = 0; i < instance.clients; i++) {
		weights.push_back(std::vector<double>());
		for (int j = 0; j < instance.facilities && instance.distribution_costs[j][i] < final_weight[i]; j++) {
			bisect_insert(weights[i], instance.distribution_costs[j][i], is_less);
		}
		weights[i].push_back(final_weight[i]);
	}
	// Initialize - we only need demand constraint weights
	semilagrange_multipliers SLR_weights = initial.RC_exact_demand;
	std::vector<int> index_set = std::vector<int>(instance.clients, 0);
	for (int i = 0; i < instance.clients; i++) {
		if (SLR_weights[i] < weights[i][0]) {
			SLR_weights[i] = weights[i][0] + epsilon;
		}
		else if (SLR_weights[i] > weights[i][weights[i].size() - 1]) {
			SLR_weights[i] = weights[i][weights[i].size() - 1] + epsilon;
			index_set[i] = weights[i].size() - 1;
		}
		else {
			for (int w = 0; w < weights[i].size(); w++) {
				if (SLR_weights[i] > weights[i][w]) {
					SLR_weights[i] = weights[i][w] + epsilon;
					index_set[i] = w;
					break;
				}
			}
		}
	}
	// Termination condition
	std::vector<double> subgradient = std::vector<double>(instance.clients, 0);
	int iter = 0;
	do {
		// Solve SLR exact
		res = solve_SLR(instance, SLR_weights);
		if (magnitude(res.subgradient_demand_constraint) == 0) {
			break;
		}
		for (int i = 0; i < instance.clients; i++) {
			if (res.subgradient_demand_constraint[i] == 1 && index_set[i] < weights[i].size() - 1) {
				index_set[i] += 1;
				SLR_weights[i] = weights[i][index_set[i]];
			}
		}
		iter++;
	} while (iter < maxIter);
	return res;
}

SLR_solution_and_subgradient solve_SLR(const SSCFLSO& instance, const semilagrange_multipliers& multipliers) {
	SLR_solution_and_subgradient res;
	GRBEnv* env = 0;
	GRBVar* open = 0;
	GRBVar** distribution = 0;
	// These are used for initializing variables
	int m = instance.facilities;
	int n = instance.clients;
	try {
		// Define the model
		env = new GRBEnv();
		GRBModel model = GRBModel(*env);
		model.set(GRB_StringAttr_ModelName, "SLR-SSCFLSO");
		// Facility variables + Bound
		open = model.addVars(m, GRB_BINARY);
		for (int j = 0; j < m; j++) {
			std::ostringstream vname;
			vname << "Facility " << j;
			open[j].set(GRB_DoubleAttr_Obj, instance.facility_costs[j]);
			open[j].set(GRB_StringAttr_VarName, vname.str());
		}
		// Distribution variables + Bound
		distribution = new GRBVar * [instance.facilities];
		for (int j = 0; j < m; j++) {
			distribution[j] = model.addVars(n, GRB_BINARY);
			for (int i = 0; i < n; i++) {
				std::ostringstream vname;
				vname << "Distribution from " << j << " to " << i;
				distribution[j][i].set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i] - multipliers[i]);
				distribution[j][i].set(GRB_StringAttr_VarName, vname.str());
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
			std::ostringstream cname;
			cname << "Demand of " << i;
			model.addConstr(totdemand <= 1, cname.str());
		}

		// Constraint capacity
		for (int j = 0; j < m; j++) {
			GRBLinExpr totdemand = 0;
			for (int i = 0; i < n; i++) {
				totdemand += instance.demands[i] * distribution[j][i];
			}
			std::ostringstream cname;
			cname << "Capacity of " << j;
			model.addConstr(totdemand <= instance.capacities[j] * open[j], cname.str());
		}

		// Constraint preference
		std::vector<std::vector<int>> rankings = std::vector<std::vector<int>>(n);
		for (int i = 0; i < n; i++) {
			rankings[i] = inverse(instance.preferences[i]);
			for (int j = 0; j < m; j++) {
				GRBLinExpr preference = open[j];
				for (int k = 0; k < m; k++) {
					if (rankings[i][k] < rankings[i][j]) {
						preference += distribution[k][i];
					}
				}
				std::ostringstream cname;
				cname << "Preference of " << i << " worse than " << j;
				model.addConstr(preference <= 1, cname.str());
			}
		}
		model.optimize();
		res;
		for (int j = 0; j < m; j++) {
			res.solution.push_back(open[j].get(GRB_DoubleAttr_X));
		}
		for (int i = 0; i < n; i++) {
			res.subgradient_demand_constraint.push_back(1);
			for (int j = 0; j < m; j++) {
				res.subgradient_demand_constraint[i] -= distribution[j][i].get(GRB_DoubleAttr_X);
			}
		}
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	assert(false);
	return res;
}


lagrange_multipliers SGM(const SSCFLSO& instance, double UB) {
	// Parameters
	int maxIter = 50;
	int noImprovementIter = 2;
	double beta = 2.0;
	double beta_reduction_factor = 2./3;
	// Initialize
	lagrange_multipliers res;
	res.RC_exact_demand = std::vector<double>(instance.clients, 0);
	for (int i = 0; i < instance.clients; i++) {
		double mincost = DBL_MAX;
		for (int j = 0; j < instance.facilities; j++) {
			double tmp = instance.distribution_costs[j][i] + instance.facility_costs[j];
			if (tmp < mincost) {
				mincost = tmp;
			}
		}
		res.RC_exact_demand[i] = mincost;
	}
	res.RC_capacity = std::vector<double>(instance.facilities, 0);
	for (int i = 0; i < instance.clients; i++) {
		res.RC_preferences.push_back(std::vector<double>(instance.facilities, 0));
	}
	// For termination condition
	int iter = 0;
	double magnitude_of_subgradient;
	// Helper variable
	double LR_comparison_value = DBL_MIN;
	int improvementIter = 0;
	do {
		LR_value_and_subgradient data = LR(instance, res);
		std::vector<double> subgradient_one_container = {};
		subgradient_one_container.insert(subgradient_one_container.begin(), data.subgradient_demand_constraint.begin(), data.subgradient_demand_constraint.end());
		subgradient_one_container.insert(subgradient_one_container.begin(), data.subgradient_capacity_constraint.begin(), data.subgradient_capacity_constraint.end());
		for (auto it = data.subgradient_preference_constraint.begin(); it != data.subgradient_preference_constraint.end(); it++) {
			subgradient_one_container.insert(subgradient_one_container.begin(), (*it).begin(), (*it).end());
		}

		magnitude_of_subgradient = magnitude(subgradient_one_container);
		if (magnitude_of_subgradient >= 10e-6) {
			// Scale weights
			double scale = beta * ((UB - data.LR_value) / (magnitude_of_subgradient * magnitude_of_subgradient));
			for (int i = 0; i < res.RC_exact_demand.size(); i++) {
				double tmp = res.RC_exact_demand[i] - scale * data.subgradient_demand_constraint[i];
				res.RC_exact_demand[i] = tmp < 0 ? 0 : tmp;
			}
			for (int j = 0; j < res.RC_exact_demand.size(); j++) {
				double tmp = res.RC_capacity[j] - scale * data.subgradient_capacity_constraint[j];
				res.RC_capacity[j] = tmp < 0 ? 0 : tmp;
			}
			for (int i = 0; i < res.RC_preferences.size(); i++) {
				for (int j = 0; j < res.RC_preferences[i].size(); j++) {
					double tmp = res.RC_preferences[i][j] - scale * data.subgradient_preference_constraint[i][j];
					res.RC_preferences[i][j] = tmp < 0 ? 0 : tmp;
				}
			}
			if (data.LR_value <= LR_comparison_value) {
				improvementIter++;
				if (improvementIter == noImprovementIter) {
					improvementIter = 0;
					beta *= beta_reduction_factor;
				}
			}
			else {
				LR_comparison_value = data.LR_value;
			}
			iter++;
		}
		else {
			return res;
		}
	} while (magnitude_of_subgradient > 10e-6 && iter < maxIter);
	return res;
}


LR_value_and_subgradient LR(const SSCFLSO& instance, const lagrange_multipliers& multipliers) {
	// For determining the subgradient at last
	std::vector<std::vector<int>> assignment_variables = {};
	std::vector<int> facility_variables = std::vector<int>(instance.facilities, 0);

	// Result - start with the LR value and then do the subgradient
	LR_value_and_subgradient res;

	// Compute coefficient of assignment variables x_ij
	for (int i = 0; i < instance.clients; i++) {
		assignment_variables.push_back(std::vector<int>(instance.facilities, 0));
		// Find cheapest tmp
		double cheapest = DBL_MAX;
		int cheapest_j = 0;
		for (int j = 0; j < instance.facilities; j++) {
			double tmp = instance.distribution_costs[j][i] + multipliers.RC_exact_demand[i] + instance.demands[i] * multipliers.RC_capacity[j];
			auto it = instance.preferences[i].begin();
			while (*it != j) {
				tmp += multipliers.RC_preferences[i][*it];
				it++;
			}
			if (tmp < cheapest) {
				cheapest = tmp;
				cheapest_j = j;
			}
		}
		assignment_variables[i][cheapest_j] = 1;
		res.LR_value += cheapest;
	}
	// Compute coefficient of facility variables y_j
	for (int j = 0; j < instance.facilities; j++) {
		double tmp = instance.facility_costs[j] - instance.capacities[j] * multipliers.RC_capacity[j];
		for (int i = 0; i < instance.clients && tmp < 0; i++) {
			tmp += multipliers.RC_preferences[i][j];
		}
		if (tmp < 0) {
			res.LR_value += tmp;
			facility_variables[j] = 1;
		}
	}
	// Constant coefficients;
	for (int i = 0; i < instance.clients; i++) {
		res.LR_value -= multipliers.RC_exact_demand[i];
		for (int j = 0; j < instance.facilities; j++) {
			res.LR_value -= multipliers.RC_preferences[i][j];
		}
	}

	// Subgradient
	for (int i = 0; i < instance.clients; i++) {
		double value = sum(assignment_variables[i]) - 1; // I think this is always equal to 0
		res.subgradient_demand_constraint.push_back(value);
	}
	for (int j = 0; j < instance.facilities; j++) {
		double value = -instance.capacities[j] * facility_variables[j];
		for (int i = 0; i < instance.clients; i++) {
			value += instance.demands[i] * assignment_variables[i][j];
		}
		res.subgradient_capacity_constraint.push_back(value - 0);
	}
	for (int i = 0; i < instance.clients; i++) {
		res.subgradient_preference_constraint.push_back(std::vector<double>());
		for (int j = 0; j < instance.facilities; j++) {
			double value = facility_variables[j];
			bool tmp_start_to_add = false;
			for (auto it = instance.preferences[i].begin(); it != instance.preferences[i].end(); it++) {
				value += tmp_start_to_add ? assignment_variables[i][*it] : 0;
				tmp_start_to_add = (*it == j) ? true : tmp_start_to_add;
			}
			res.subgradient_preference_constraint[i].push_back(value - 1);
		}
	}
	return res;
}