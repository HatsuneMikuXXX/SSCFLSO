#include "algorithmClass.h"

Algorithm::UPDATE_CODE Algorithm::improve_solution(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& new_solution, Timer& timer, ReportResult& report, bool searchingForFeasibleSolution = false) {
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
	if (searchingForFeasibleSolution) {
		// Use Rating
		if (FLV.evaluate_inf_solution() < current_best.val || current_best.val == 1) {
			report.evalResult(forTheReport, timer);
			timer.proceed_with_timer();
			current_best.sol = new_solution;
			return Algorithm::IMPROVED;
		}
	}
	else if (FLV.value() < current_best.val || current_best.val == -1) {
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
	Validator FLV;
	Timer* timer;
	ReportResult* report;
	GRBVar* open;
	int numFacilities;
	solution_and_value* current_best;
	myCallback(GRBVar* open, const SSCFLSO& instance, solution_and_value& current_best,Timer& timer, ReportResult& report) {
		this->open = open;
		this->FLV = Validator(instance);
		this->numFacilities = instance.facilities;
		this->timer = &timer;
		this->report = &report;
		this->current_best = &current_best;
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
				this->FLV.set_solution(solution);
				if (this->FLV->feasible() && this->FLV.value() < this->current_best->val && timer->in_time()) {
					this->current_best->sol = solution;
					this->current_best->val = FLV.value();
					this->report->evalResult(*this->current_best, *this->timer);
				}
				std::cout << "Solution value: " << this->FLV.value() << std::endl;
			}
		}
		catch (GRBException e) {
			std::cout << "Error during callback." << std::endl;
			std::cout << e.getMessage() << std::endl;
		}
		timer->proceed_with_timer();
	}
};

void Algorithm::solve_with_gurobi_afterwards(const SSCFLSO& instance, solution_and_value& current_best, const facility_vector& initial, Timer& timer, ReportResult& report) {
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
		// Objective
		model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

		// Constraints
		{
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
		}
		// Initial solution
		for (int j = 0; j < m; j++) {
			open[j].set(GRB_DoubleAttr_Start, initial[j]);
		}
		// Callback - Required for frequent updates
		myCallback cb = myCallback(open, instance, current_best, timer, report);
		model.setCallback(&cb);
		// Start solving
		double remaining_time_limit_in_sec = timer.get_remaining_time() / 1000;
		model.set(GRB_DoubleParam_TimeLimit, remaining_time_limit_in_sec);
		model.optimize();
		report.finishUp();
		return;
	}
	catch (std::exception e) {
		std::cerr << "Something went wrong:\n" << e.what() << std::endl;
	}
	catch (GRBException e) {
		std::cerr << "Something went wrong:\n" << e.getMessage() << std::endl;
	}
}
