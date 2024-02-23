#include "semi_lagrangian_relaxation.h"

class SemiLagrangianRelaxation : public Algorithm {
public:
	static void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) {
		auto start = start_timer();
		Validator FLV = Validator(instance);
		facility_vector UB;
		Stingy::solve(instance, UB, time_limit);
		FLV.set_solution(UB);
		lagrangian_multipliers initial = SGM(instance, FLV.value()); // Solve LR to get initial weights
		// Use DAM and repeatedly solve SLR. Warning: Solution might not be feasible if timelimit is exceeded!
		SLR_solution_and_subgradient intermediate = DAM(instance, initial, start, time_limit, current_best); 
		if (within_time_limit(start, time_limit)) {
			current_best = intermediate.solution;
		}
	};

	

	static std::string meta_information(){ 
		std::ifstream file(PATH + "semi_lagrangian_relaxation/semi_lagrangian_relaxation.txt");
		if (file) {
			std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
			return content;
		}
		else {
			throw std::runtime_error("Semi Lagrangian Relaxation information file not found.");
		}
	}

private:
	static lagrangian_multipliers SGM(const SSCFLSO& instance, double UB) {
		// Parameters
		int maxIter = 50;
		int noImprovementIter = 2;
		double beta = 2.0;
		double beta_reduction_factor = 2. / 3;

		// Initialize
		lagrangian_multipliers res;
		res.demand_constraint_weight = std::vector<double>(instance.clients, 0);
		for (int i = 0; i < instance.clients; i++) {
			double mincost = DBL_MAX;
			for (int j = 0; j < instance.facilities; j++) {
				double tmp = instance.distribution_costs[j][i] + instance.facility_costs[j];
				if (tmp < mincost) {
					mincost = tmp;
				}
			}
			res.demand_constraint_weight[i] = mincost;
		}
		res.capacity_constraint_weight = std::vector<double>(instance.facilities, 0);
		for (int i = 0; i < instance.clients; i++) {
			res.preference_constraint_weight.push_back(std::vector<double>(instance.facilities, 0));
		}

		// For termination condition
		int iter = 0;
		double magnitude_of_subgradient;
		// Helper variable
		double LR_comparison_value = DBL_MIN;
		int improvementIter = 0;
		do {
			// Iteratively solve LR and gather LR value and the subgradient
			LR_value_and_subgradient data = LR(instance, res);
			std::vector<double> subgradient_single_container = {};
			subgradient_single_container.insert(subgradient_single_container.begin(), data.subgradient_demand_constraint.begin(), data.subgradient_demand_constraint.end());
			subgradient_single_container.insert(subgradient_single_container.begin(), data.subgradient_capacity_constraint.begin(), data.subgradient_capacity_constraint.end());
			for (auto it = data.subgradient_preference_constraint.begin(); it != data.subgradient_preference_constraint.end(); it++) {
				subgradient_single_container.insert(subgradient_single_container.begin(), (*it).begin(), (*it).end());
			}
			// Compute the magnitude of the subgradient, the closer to 0 the closer we are to an optimum of the LR
			magnitude_of_subgradient = magnitude(subgradient_single_container);
			if (magnitude_of_subgradient >= 10e-6) {
				// Scale Lagrangian weights
				double scale = beta * ((UB - data.LR_value) / (magnitude_of_subgradient * magnitude_of_subgradient));
				for (int i = 0; i < res.demand_constraint_weight.size(); i++) {
					double tmp = res.demand_constraint_weight[i] - scale * data.subgradient_demand_constraint[i];
					res.demand_constraint_weight[i] = tmp < 0 ? 0 : tmp;
				}
				for (int j = 0; j < res.demand_constraint_weight.size(); j++) {
					double tmp = res.capacity_constraint_weight[j] - scale * data.subgradient_capacity_constraint[j];
					res.capacity_constraint_weight[j] = tmp < 0 ? 0 : tmp;
				}
				for (int i = 0; i < res.preference_constraint_weight.size(); i++) {
					for (int j = 0; j < res.preference_constraint_weight[i].size(); j++) {
						double tmp = res.preference_constraint_weight[i][j] - scale * data.subgradient_preference_constraint[i][j];
						res.preference_constraint_weight[i][j] = tmp < 0 ? 0 : tmp;
					}
				}
				// Check if we improved. If not then adjust beta value to speed up the process
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

	static LR_value_and_subgradient LR(const SSCFLSO& instance, const lagrangian_multipliers& multipliers) {
		LR_value_and_subgradient res;
		extended_solution exso = solve_lagrangian_relaxation(instance, multipliers);
		res.LR_value = 0;
		for (int j = 0; j < instance.facilities; j++) {
			res.LR_value += (exso.ES_facilities[j] == 1) ? instance.facility_costs[j] : 0;
		}
		for (int j = 0; j < instance.facilities; j++) {
			for (int i = 0; i < instance.clients; i++) {
				res.LR_value += (exso.ES_extended_assignment[i][j] == 1) ? instance.distribution_costs[j][i] : 0;
			}
		}
		// Subgradient
		// Demand Constraint
		for (int i = 0; i < instance.clients; i++) {
			double value = sum(exso.ES_extended_assignment[i]) - 1; // I think this is always equal to 0
			res.subgradient_demand_constraint.push_back(value);
		}
		// Capacity Constraint
		for (int j = 0; j < instance.facilities; j++) {
			double value = -instance.capacities[j] * exso.ES_facilities[j];
			for (int i = 0; i < instance.clients; i++) {
				value += instance.demands[i] * exso.ES_extended_assignment[i][j];
			}
			res.subgradient_capacity_constraint.push_back(value - 0);
		}
		// Preference Constraint
		for (int i = 0; i < instance.clients; i++) {
			res.subgradient_preference_constraint.push_back(std::vector<double>());
			for (int j = 0; j < instance.facilities; j++) {
				double value = exso.ES_facilities[j];
				bool tmp_start_to_add = false;
				for (auto it = instance.preferences[i].begin(); it != instance.preferences[i].end(); it++) {
					value += tmp_start_to_add ? exso.ES_extended_assignment[i][*it] : 0;
					tmp_start_to_add = (*it == j) ? true : tmp_start_to_add;
				}
				res.subgradient_preference_constraint[i].push_back(value - 1);
			}
		}
		return res;
	}

	static SLR_solution_and_subgradient DAM(
		const SSCFLSO& instance, 
		const lagrangian_multipliers& initial, 
		const std::chrono::system_clock::time_point& start,
		const std::chrono::milliseconds& time_limit,
		facility_vector& current_best) {
		SLR_solution_and_subgradient res;
		// Parameters
		int maxIter = 50;
		double epsilon = 5 * EPSILON;

		// Find maximal weight for each client i, that is max_j{c_ji + f_j} (that way we allow every combination of client to facility)
		std::vector<double> final_weight = std::vector<double>(instance.clients, 0); 
		for (int i = 0; i < instance.clients; i++) {
			for (int j = 0; j < instance.facilities; j++) {
				double tmp = instance.facility_costs[j] + instance.distribution_costs[j][i];
				final_weight[i] = (tmp > final_weight[i]) ? tmp : final_weight[i];
			}
		}
		// Construct possible weight values (Remember: weights are updated in DAM)
		std::vector<std::vector<double>> weights = {};
		std::function<bool(double, double)> is_less = [](double a, double b) { return a < b; };
		for (int i = 0; i < instance.clients; i++) {
			weights.push_back(std::vector<double>());
			for (int j = 0; j < instance.facilities && instance.distribution_costs[j][i] < final_weight[i]; j++) {
				bisect_insert(weights[i], instance.distribution_costs[j][i], is_less);
			}
			weights[i].push_back(final_weight[i]);
		}
		// Initialize weights - we only need demand constraint weights
		semilagrange_multipliers SLR_weights = initial.demand_constraint_weight;
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
		// Now the actual process
		do {
			// Solve SLR exact
			res = SLR(instance, SLR_weights);
			if (!within_time_limit(start, time_limit)) {
				return res;
			}
			else {
				current_best = res.solution;
			}
			if (magnitude(res.subgradient_demand_constraint) == 0) {
				break;
			}
			// Update weights
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

	static SLR_solution_and_subgradient SLR(const SSCFLSO& instance, const semilagrange_multipliers& multipliers) {
		SLR_solution_and_subgradient res;
		int m = instance.facilities;
		int n = instance.clients;
		try {
			// Define the model
			GRBEnv* env = new GRBEnv();
			GRBModel model = GRBModel(*env);
			env->set(GRB_IntParam_OutputFlag, 0); // Surpress Console
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
					distribution[j][i].set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i] - multipliers[i]);
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
					model.addConstr(preference <= 1);
				}
			}
			model.optimize();
			for (int j = 0; j < m; j++) {
				res.solution.push_back(open[j].get(GRB_DoubleAttr_X));
			}
			for (int i = 0; i < n; i++) {
				res.subgradient_demand_constraint.push_back(1);
				for (int j = 0; j < m; j++) {
					res.subgradient_demand_constraint[i] -= distribution[j][i].get(GRB_DoubleAttr_X);
				}
			}
			return res;
		}
		catch (std::exception e) {
			std::cerr << "Something went wrong:\n" << e.what() << std::endl;
		}
	}
};