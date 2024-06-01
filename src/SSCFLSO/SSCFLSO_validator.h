#ifndef VALIDATOR_H
#define VALIDATOR_H
#include "../common.h"
#include "../helpers/utility.h"
#include "SSCFLSO.h"
/// @brief Everything solution-related: Computing objective value, checking feasilibilty with witness.
class Validator{
	public:
		Validator(const SSCFLSO&);
		Validator operator=(const Validator) {
			return *this;
		};
		void set_solution(const facility_vector&);
		const facility_vector& get_solution();
		const client_facility_assignment& get_assignment();
		double value();
		bool feasible();
		// A binary facility vector of all facilities that exceed their capacity
		facility_vector exceeds_capacity();
		// Heuristic to rate infeasible solution. Minimizing rating should lead to a feasible solution.
		double evaluate_inf_solution();
		// Remove facilities that serve no one
		void drop_empty_facilities();
	private:
		const SSCFLSO& ref_instance;
		// Solution is a binary vector
		facility_vector solution;
		double solution_value;
		//Index = Facility, Value = Client
		client_facility_assignment assignment;
		alreadyComputed<bool> Feasibility;
		alreadyComputed<double> Rating;
};
#endif