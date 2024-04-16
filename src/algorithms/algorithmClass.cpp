#include "algorithmClass.h"

/**
	* Removes unnecessary facilities which are:
	* - Type II: Facilities that when opened always cause the solution to be infeasible
	* - Type I: Facilities that cannot even serve one client
	* - Type III: Facilities that are never assigned because they are too unpopular
*/

std::chrono::system_clock::time_point Algorithm::start_timer() {
	return std::chrono::system_clock::now();
}

std::chrono::milliseconds Algorithm::get_elapsed_time_ms(const std::chrono::system_clock::time_point& start) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
}

bool Algorithm::within_time_limit(const std::chrono::system_clock::time_point& start, const std::chrono::milliseconds& time_limit) {
	return (get_elapsed_time_ms(start).count() < time_limit.count());
}

facility_vector Algorithm::preprocess(const SSCFLSO& instance) {
	Validator FLV = Validator(instance);
	facility_vector no_unnecessary_facilities = facility_vector(instance.facilities, 1);
	{
		// Remove facilities of Type II
		FLV.set_solution(no_unnecessary_facilities);
		{
			facility_vector exceeds_capacity;
			while (!FLV.feasible() && sum(no_unnecessary_facilities) > 0) {
				// Subtract facilities that exceed capacity
				exceeds_capacity = FLV.exceeds_capacity();
				std::transform(no_unnecessary_facilities.begin(), no_unnecessary_facilities.end(), exceeds_capacity.begin(), no_unnecessary_facilities.begin(), std::minus<int>());
				FLV.set_solution(no_unnecessary_facilities);
			}
		}
	}
	{
		// Remove facilities of Type I
		double minimum_demand = *min_element(instance.demands.begin(), instance.demands.end());
		capacity_vector capacities = instance.capacities;
		facility_predicate predicate = [capacities, minimum_demand](int facility) { return capacities[facility] >= minimum_demand; };
		filter(no_unnecessary_facilities, predicate);
	}
	{
		// Remove facilities of Type III
		bool fixpoint_reached = false;
		while (!fixpoint_reached) {
			fixpoint_reached = true;
			int min = lower_bound_facilities_required(instance, no_unnecessary_facilities);
			if (min == -1) {
				break;
			}
			// Compute rankings
			std::vector<std::map<int, int>> rankings;
			for (int i = 0; i < instance.clients; i++) {
				std::map<int, int> ranking = {};
				int rank = 0;
				for (auto it = instance.preferences[i].begin(); it != instance.preferences[i].end(); it++) {
					if (no_unnecessary_facilities[*it] == 1) {
						ranking[*it] = rank;
						rank++;
					}
				}
				rankings.push_back(ranking);
			}
			// Find too unpopular facilities
			facility_vector typeIII_facilities = no_unnecessary_facilities;
			int typeIII_count = sum(typeIII_facilities);
			for (int i = 0; i < instance.clients && typeIII_count > 0; i++) {
				for (int j = 0; j < instance.facilities; j++) {
					if (typeIII_facilities[j] == 0) { continue; }
					if (rankings[i][j] <= instance.facilities - min) {
						typeIII_facilities[j] = 0;
						typeIII_count -= 1;
					}
				}
			}
			if (typeIII_count == 0) {
				break;
			}
			// Update
			for (int j = 0; j < instance.facilities; j++) {
				if (typeIII_facilities[j] == 1) {
					no_unnecessary_facilities[j] = 0;
					fixpoint_reached = false;
				}
			}
		}	
	}
	return no_unnecessary_facilities;
}

int Algorithm::lower_bound_facilities_required(const SSCFLSO& instance, const facility_vector& facilities_to_consider) {
	// Collect capacities in a sorted order
	const std::function<bool(const double&, const double&)> is_greater = [](const double& a, const double& b) { return a > b; };
	capacity_vector sorted_capacities = {};
	for (int j = 0; j < instance.facilities; j++) {
		if (facilities_to_consider[j] == 0) { continue; }
		bisect_insert(sorted_capacities, instance.capacities[j], is_greater);
	}
	// Compute minimum amount of facilities required
	double total_demand = sum(instance.demands);
	int min = 0;
	double min_capacity = 0;
	for (auto it = sorted_capacities.begin(); it != sorted_capacities.end() && min_capacity < total_demand; it++) {
		min += 1;
		min_capacity += *it;
	}
	if (min_capacity < total_demand) {
		return -1;
	}
	return min;
}

std::vector<facility_vector> Algorithm::add_neighborhood(const facility_vector& x, const facility_vector& preprocessed) {
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

std::vector<facility_vector> Algorithm::remove_neighborhood(const facility_vector& x) {
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

std::vector<facility_vector> Algorithm::swap_neighborhood(const facility_vector& x, const facility_vector& preprocessed) {
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



relaxed_solution Algorithm::solve_linear_relaxation(const SSCFLSO& instance, const eliminate_variables& variables, const initial_solution* init) {
	relaxed_solution res = relaxed_solution();
	int m = instance.facilities;
	int n = instance.clients;
	try {
		// Define the model
		GRBEnv* env = new GRBEnv();
		env->set(GRB_IntParam_OutputFlag, 0); // Surpress Console
		GRBModel model = GRBModel(*env);
		// Facility variables
		GRBVar* open = model.addVars(m, GRB_CONTINUOUS);
		for (int j = 0; j < m; j++) {
			open[j].set(GRB_DoubleAttr_Obj, instance.facility_costs[j]);
		}
		// Distribution variables
		GRBVar** distribution = new GRBVar * [instance.facilities];
		for (int j = 0; j < m; j++) {
			distribution[j] = model.addVars(n, GRB_CONTINUOUS);
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
		// Bounds/Variables to eliminate
		for (int j = 0; j < m; j++) {
			if (variables.facilities_keep_closed[j] == 1) {
				open[j].set(GRB_DoubleAttr_UB, 0);
			}
			else {
				open[j].set(GRB_DoubleAttr_UB, 1);
			}
		}
		for (int j = 0; j < m; j++) {
			for (int i = 0; i < n; i++) {
				if (variables.distribution_keep_closed[j][i] == 1) {
					distribution[j][i].set(GRB_DoubleAttr_UB, 0);
				}
				else {
					distribution[j][i].set(GRB_DoubleAttr_UB, 1);
				}
			}
		}

		// Initial solution
		if (init != NULL) {
			for (int j = 0; j < m; j++) {
				open[j].set(GRB_DoubleAttr_Start, init->initial_facilities[j]);
			}
			for (int j = 0; j < m; j++) {
				for (int i = 0; i < n; i++) {
					distribution[j][i].set(GRB_DoubleAttr_Start, init->initial_distribution[j][i]);
				}
			}
		}
		auto start = start_timer();
		model.optimize();
		res.rFacility_vector = std::vector<double>(m, -1);
		for (int j = 0; j < m; j++) {
			res.rFacility_vector[j] = open[j].get(GRB_DoubleAttr_X);
			res.rAssignment_vector.push_back(std::vector<double>(n, -1));
			for (int i = 0; i < n; i++) {
				res.rAssignment_vector[j][i] = distribution[j][i].get(GRB_DoubleAttr_X);
			}
		}
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	catch (GRBException e) {
		std::cerr << "Something went wrong:\n" << e.getMessage() << std::endl;
	}
	return res;
}

facility_vector Algorithm::solve_with_gurobi(const SSCFLSO& instance, const std::chrono::milliseconds& time_limit, const facility_vector& initial_solution) {
	auto start = std::chrono::system_clock::now();
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
		for (int j = 0; j < m; j++) {
			open[j].set(GRB_DoubleAttr_Start, initial_solution[j]);
		}
		auto subtract = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
		model.set(GRB_DoubleParam_TimeLimit, (double((time_limit - subtract).count()) / 1000) - 5);
		model.optimize();
		facility_vector solution = facility_vector(instance.facilities, -1);
		for (int j = 0; j < m; j++) {
			solution[j] = open[j].get(GRB_DoubleAttr_X);
		}
		return solution;
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	catch (GRBException e) {
		std::cerr << "Something went wrong:\n" << e.getMessage() << std::endl;
	}
}
