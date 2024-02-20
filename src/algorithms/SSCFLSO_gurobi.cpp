#include "SSCFLSO_gurobi.h"

relaxed_solution solve(const SSCFLSO& instance){
	relaxed_solution res = relaxed_solution();
	GRBEnv* env = 0;
	GRBVar* open = 0;
	GRBVar** distribution = 0;
	// These are used for initializing variables
	int m = instance.facilities;
	int n = instance.clients;
	try{
		// Define the model
		env = new GRBEnv();
		GRBModel model = GRBModel(*env);
		model.set(GRB_StringAttr_ModelName, "SSCFLSO");
		// Facility variables + Bound
		open = model.addVars(m, GRB_CONTINUOUS);
		for(int j = 0; j < m; j++){
			std::ostringstream vname;
			vname << "Facility " << j;
			open[j].set(GRB_DoubleAttr_Obj, instance.facility_costs[j]);
			open[j].set(GRB_StringAttr_VarName, vname.str());
			model.addConstr(open[j] <= 1);
		}
		// Distribution variables + Bound
		distribution = new GRBVar* [instance.facilities];
		for(int j = 0; j < m; j++){
			distribution[j] = model.addVars(n, GRB_CONTINUOUS);
			for(int i = 0; i < n; i++){
				std::ostringstream vname;
				vname << "Distribution from " << j << " to " << i;
				distribution[j][i].set(GRB_DoubleAttr_Obj, instance.distribution_costs[j][i]);
				distribution[j][i].set(GRB_StringAttr_VarName, vname.str());
				model.addConstr(distribution[j][i] <= 1);
			}
		}
		// Objective
		model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

		// Constraint demand
		for(int i = 0; i < n; i++){
			GRBLinExpr totdemand = 0;
			for(int j = 0; j < m; j++){
				totdemand += distribution[j][i];
			}
			std::ostringstream cname;
			cname << "Demand of " << i;
			model.addConstr(totdemand == 1, cname.str());
		}

		// Constraint capacity
		for(int j = 0; j < m; j++){
			GRBLinExpr totdemand = 0;
			for(int i = 0; i < n; i++){
				totdemand += instance.demands[i] * distribution[j][i];
			}
			std::ostringstream cname;
			cname << "Capacity of " << j;
			model.addConstr(totdemand <= instance.capacities[j] * open[j], cname.str());
		}

		// Constraint preference
		std::vector<std::vector<int>> rankings = std::vector<std::vector<int>>(n);
		for(int i = 0; i < n; i++){
			rankings[i] = inverse(instance.preferences[i]);
			for(int j = 0; j < m; j++){
				GRBLinExpr preference = open[j];
				for(int k = 0; k < m; k++){
					if(rankings[i][k] < rankings[i][j]){
						preference += distribution[k][i];
					}
				}
				std::ostringstream cname;
				cname << "Preference of " << i << " worse than " << j;
				model.addConstr(preference <= 1, cname.str());
			}
		}
		model.optimize();
		for (int j = 0; j < m; j++) {
			res.rFacility_vector.push_back(open[j].get(GRB_DoubleAttr_X));
			res.rFacilityClient_vector[j] = {};
			for (int i = 0; i < n; i++) {
				res.rFacilityClient_vector[j].push_back(distribution[j][i].get(GRB_DoubleAttr_X));
			}
		}
	}
	catch(std::exception e){
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	return res;
}