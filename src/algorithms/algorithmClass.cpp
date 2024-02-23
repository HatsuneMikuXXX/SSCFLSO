#include "algorithmClass.h"

class Algorithm {
	const std::string Algorithm::PATH = "src/algorithms/";
	/**
	 * Removes unnecessary facilities which are:
	 * - Type I: Facilities that when opened always cause the solution to be infeasible
	 * - Type II: Facilities that cannot even serve one client
	*/

	static std::chrono::system_clock::time_point start_timer() {
		return std::chrono::system_clock::now();
	}

	static std::chrono::milliseconds get_elapsed_time_ms(const std::chrono::system_clock::time_point& start) {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
	}

	static bool within_time_limit(const std::chrono::system_clock::time_point& start, const std::chrono::milliseconds& time_limit) {
		return (get_elapsed_time_ms(start).count() < time_limit.count());
	}

	static facility_vector preprocess(const SSCFLSO& instance) {
		Validator FLV = Validator(instance);
		facility_vector no_unnecessary_facilities = facility_vector(instance.facilities, 1);
		FLV.set_solution(no_unnecessary_facilities);
		// Remove facilities of Type I
		{
			facility_vector exceeds_capacity;
			while (!FLV.feasible() && sum(no_unnecessary_facilities) > 0) {
				// Subtract facilities that exceed capacity
				exceeds_capacity = FLV.exceeds_capacity();
				std::transform(no_unnecessary_facilities.begin(), no_unnecessary_facilities.end(), exceeds_capacity.begin(), no_unnecessary_facilities.begin(), std::minus<int>());
				FLV.set_solution(no_unnecessary_facilities);
			}
		}
		// Remove facilities of Type II
		double minimum_demand = *min_element(instance.demands.begin(), instance.demands.end());
		capacity_vector capacities = instance.capacities;
		facility_predicate predicate = [capacities, minimum_demand](int facility) { return capacities[facility] >= minimum_demand; };
		filter(no_unnecessary_facilities, predicate);
		return no_unnecessary_facilities;
	}

	static std::vector<facility_vector> add_neighborhood(const facility_vector& x, const facility_vector& preprocessed) {
		std::vector<facility_vector> res = {};
		facility_vector neighbor;
		for (int i = 0; i < x.size(); i++) {
			if (preprocessed[i] == 1 && x[i] == 0) {
				neighbor = x;
				neighbor[i] = 1;
				res.push_back(neighbor);
			}
		}
		return res;
	}

	static std::vector<facility_vector> remove_neighborhood(const facility_vector& x) {
		std::vector<facility_vector> res = {};
		facility_vector neighbor;
		for (int i = 0; i < x.size(); i++) {
			if (x[i] == 1) {
				neighbor = x;
				neighbor[i] = 0;
				res.push_back(neighbor);
			}
		}
		return res;
	}

	static std::vector<facility_vector> swap_neighborhood(const facility_vector& x, const facility_vector& preprocessed) {
		std::vector<facility_vector> res = {};
		facility_vector neighbor;
		for (int i = 0; i < x.size(); i++) {
			if (preprocessed[i] == 1 && x[i] == 0) {
				for (int j = 0; j < x.size(); j++) {
					if (x[j] == 1) {
						neighbor = x;
						neighbor[i] = 1;
						neighbor[j] = 0;
						res.push_back(neighbor);
					}
				}
			}
		}
		return res;
	}

	static relaxed_solution solve_linear_relaxation(const SSCFLSO& instance) {
		relaxed_solution res = relaxed_solution();
		int m = instance.facilities;
		int n = instance.clients;
		try {
			// Define the model
			GRBEnv* env = new GRBEnv();
			GRBModel model = GRBModel(*env);
			env->set(GRB_IntParam_OutputFlag, 0); // Surpress Console
			// Facility variables + Bound
			GRBVar* open = model.addVars(m, GRB_CONTINUOUS);
			for (int j = 0; j < m; j++) {
				open[j].set(GRB_DoubleAttr_Obj, instance.facility_costs[j]);
				model.addConstr(open[j] <= 1);
			}
			// Distribution variables + Bound
			GRBVar** distribution = new GRBVar * [instance.facilities];
			for (int j = 0; j < m; j++) {
				distribution[j] = model.addVars(n, GRB_CONTINUOUS);
				for (int i = 0; i < n; i++) {
					distribution[j][i].set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i]);
					model.addConstr(distribution[j][i] <= 1);
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
						if (rankings[i][k] < rankings[i][j]) {
							preference += distribution[k][i];
						}
					}
					model.addConstr(preference <= 1);
				}
			}
			model.optimize();
			for (int j = 0; j < m; j++) {
				res.rFacility_vector.push_back(open[j].get(GRB_DoubleAttr_X));
				res.rAssignment_vector[j] = {};
				for (int i = 0; i < n; i++) {
					res.rAssignment_vector[j].push_back(distribution[j][i].get(GRB_DoubleAttr_X));
				}
			}
		}
		catch (std::exception e) {
			std::cerr << "Something went wrong:\n" << e.what() << std::endl;
		}
		return res;
	}

	static extended_solution solve_lagrangian_relaxation(const SSCFLSO& instance, const lagrangian_multipliers& multipliers) {
		extended_solution res;
		res.ES_facilities = facility_vector(instance.facilities, -1);

		int m = instance.facilities;
		int n = instance.clients;
		
		assert(n == multipliers.demand_constraint_weight.size());
		assert(m == multipliers.capacity_constraint_weight.size());
		for (int i = 0; i < n; i++) {
			assert(m == multipliers.preference_constraint_weight[i].size());
		}
		
		try {
			// Define the model
			GRBEnv* env = new GRBEnv();
			GRBModel model = GRBModel(*env);
			env->set(GRB_IntParam_OutputFlag, 0); // Surpress Console
			// Facility variables + Bound
			GRBVar* open = model.addVars(m, GRB_BINARY);
			for (int j = 0; j < m; j++) {
				double coefficient = instance.facility_costs[j] - instance.capacities[j] * multipliers.capacity_constraint_weight[j];
				for (int i = 0; i < n; i++) {
					coefficient += multipliers.preference_constraint_weight[i][j];
				}
				open[j].set(GRB_DoubleAttr_Obj, coefficient);
			}
			// Distribution variables + Bound
			GRBVar** distribution = new GRBVar * [instance.facilities];
			for (int j = 0; j < m; j++) {
				distribution[j] = model.addVars(n, GRB_BINARY);
				for (int i = 0; i < n; i++) {
					double coefficient = instance.distribution_costs[j][i] + multipliers.demand_constraint_weight[i] + instance.demands[i] * multipliers.capacity_constraint_weight[i];
					for (auto k = instance.preferences[i].begin(); k != instance.preferences[i].end(); k++) {
						if (*k == j) { break; }
						coefficient += multipliers.preference_constraint_weight[i][*k];
					}
					distribution[j][i].set(GRB_DoubleAttr_Obj, coefficient);
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
				model.addConstr(totdemand >= 1);
			}
			model.optimize();
			for (int j = 0; j < m; j++) {
				res.ES_facilities[j] = (open[j].get(GRB_DoubleAttr_X));
			}
			for (int i = 0; i < n; i++) {
				res.ES_extended_assignment.push_back(std::vector<int>(instance.facilities, -1));
				for (int j = 0; j < m; j++) {
					res.ES_extended_assignment[i][j] = (distribution[j][i].get(GRB_DoubleAttr_X));
				}
			}
		}
		catch (std::exception e) {
			std::cerr << "Something went wrong:\n" << e.what() << std::endl;
		}
		return res;
	}
};

