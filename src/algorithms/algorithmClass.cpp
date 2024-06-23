#include "algorithmClass.h"

Algorithm::UPDATE_CODE Algorithm::improve_solution(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& new_solution, Timer& timer, ReportResult& report) {
	if (!timer.running_status()) {
		return Algorithm::TIMER_NOT_RUNNING;
	}
	timer.pause_timer();
	if (!timer.in_time()) {
		return Algorithm::TIMEOUT;
	}
	Validator FLV(instance);
	FLV.set_solution(new_solution);
	solution_and_value forTheReport = { new_solution, FLV.value() };
	if (FLV.value() < current_best.val || current_best.val == -1) {
		// Use Value
		report.evalResult(forTheReport, timer);
		timer.proceed_with_timer();
		current_best.sol = new_solution;
		return Algorithm::IMPROVED;
	}
	timer.proceed_with_timer();
	return Algorithm::NOT_IMPROVED;
}

class myCallback : public GRBCallback {
public:
	GRBVar* open;
	Validator* FLV;
	int numFacilities;
	solution_and_value* current_best;
	Timer* timer;
	ReportResult* report;
	myCallback(GRBVar* open, Validator& FLV, const int numFacilities, solution_and_value& current_best, Timer& timer, ReportResult& report) {
		this->open = open;
		this->FLV = &FLV;
		this->numFacilities = numFacilities;
		this->current_best = &current_best;
		this->timer = &timer;
		this->report = &report;
		
	}
protected:
	void callback() {
		timer->pause_timer();
		try {
			if (where == GRB_CB_MIPSOL) {
				facility_vector solution(numFacilities, -1);
				for (int j = 0; j < this->numFacilities; j++) {
					solution[j] = open[j].get(GRB_DoubleAttr_X);
				}
				this->FLV->set_solution(solution);
				if (this->FLV->feasible() && this->FLV->value() < this->current_best->val && timer->in_time()) {
					this->current_best->sol = solution;
					this->current_best->val = FLV->value();
					this->report->evalResult(*this->current_best, *this->timer);
				}
				std::cout << "Solution value: " << this->FLV->value() << std::endl;
			}
		}
		catch (GRBException e) {
			std::cout << "Error during callback." << std::endl;
			std::cout << e.getMessage() << std::endl;
		}
		timer->proceed_with_timer();
	}
};

void Algorithm::solve_with_gurobi_afterwards(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& initial, Timer& timer, ReportResult& report) const {
	const range_vector facility_range = range(instance.facilities);
	const range_vector client_range = range(instance.clients);
	const int m = facility_range.size();
	const int n = client_range.size();
	try {
		// Define the model
		GRBEnv* env = new GRBEnv();
		env->set(GRB_IntParam_OutputFlag, 0); // Surpress Console
		GRBModel model = GRBModel(*env);
		// Facility variables
		GRBVar* open = model.addVars(m, GRB_BINARY);
		asa::for_each(facility_range, [&open, &instance](const int facility_id) {
			open[facility_id].set(GRB_DoubleAttr_Obj, instance.facility_costs[facility_id]);
			});
		// Distribution variables
		GRBVar** distribution = new GRBVar * [instance.facilities];
		asa::for_each(facility_range, [&distribution, &model, &n, &client_range, &instance](const int facility_id) {
			distribution[facility_id] = model.addVars(n, GRB_BINARY);
			asa::for_each(client_range, [&distribution, &instance, &facility_id](const int client_id) {
				distribution[facility_id][client_id].set(GRB_DoubleAttr_Obj, instance.distribution_costs[facility_id][client_id]);
			});
		});
		// Objective
		model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

		// Constraints
		{
			// Constraint demand
			asa::for_each(client_range, [&facility_range, &distribution, &model](const int client_id) {
				GRBLinExpr totdemand = 0;
				asa::for_each(facility_range, [&totdemand, &distribution, &client_id](const int facility_id) {
					totdemand += distribution[facility_id][client_id];
				});
				model.addConstr(totdemand == 1);
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
		}
		// Initial solution
		asa::for_each(facility_range, [&open, &initial](const int facility_id) {open[facility_id].set(GRB_DoubleAttr_Start, initial[facility_id]); });

		// Callback - Required for frequent updates
		Validator FLV = Validator(instance);
		myCallback cb = myCallback(open, FLV, instance.facilities, current_best, timer, report);
		model.setCallback(&cb);
		// Start solving
		double remaining_time_limit_in_sec = timer.get_remaining_time() / 1000;
		model.set(GRB_DoubleParam_TimeLimit, remaining_time_limit_in_sec);
		model.optimize();
		return;
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	catch (GRBException e) {
		std::cerr << "Something went wrong:\n" << e.getMessage() << std::endl;
	}
}
