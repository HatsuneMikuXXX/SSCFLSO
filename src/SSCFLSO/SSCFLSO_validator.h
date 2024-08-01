#ifndef VALIDATOR_H
#define VALIDATOR_H
#include "../common.h"
#include "../helpers/utility.h"
#include "SSCFLSO_generator.h"
#include <cassert>

typedef std::vector<bool> facility_vector;
typedef std::vector<int> client_facility_assignment;

/// @brief Everything solution-related: Computing objective value, checking feasilibilty with witness.
class Validator{
public:
		Validator(const SSCFLSO& ref);
		Validator operator=(const Validator) {
			return *this;
		};
		void set_solution(const facility_vector&);
		const facility_vector& get_solution() const;
		const client_facility_assignment& get_assignment() const;
		double value() const;
		bool feasible();
		// A binary facility vector of all facilities that exceed their capacity
		facility_vector exceeds_capacity();
		// Heuristic to rate infeasible solution. Minimizing rating should lead to a feasible solution.
		double evaluate_inf_solution();
		// Remove facilities that serve no one
		void drop_empty_facilities();
	private:
		const SSCFLSO& ref_instance;
		facility_vector solution; // Solution is a binary vector
		double solution_value{-1};
		client_facility_assignment assignment; //Index = Facility, Value = Client

		bool is_feasible{false};
		bool feasibility_already_computed{true}; // Starting with no facility open
		double infeasible_solution_rating{-1};
		bool infeasible_solution_rating_already_computed{false};
		bool assignment_already_computed{ false };
};

// For the algorithms to use
struct solution_and_value {
	facility_vector sol;
	double val;
};

// Store multiple solutions in an efficient manner
class SolutionContainer {
public:
	SolutionContainer(const facility_vector& root);
	void add(const facility_vector& next);
	bool contains(const facility_vector& val) const;
private:
	const facility_vector root;
	facility_vector tail;
	std::vector<int> change_log{ std::vector<int>(0) };
};

#endif