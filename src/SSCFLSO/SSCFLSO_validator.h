#ifndef VALIDATOR_H
#define VALIDATOR_H
#include <functional> // Required in the violation check.
#include "../helpers/data_helper.h"
#include "SSCFLSO.h"
/// @brief Everything solution-related: Computing objective value, checking feasilibilty with witness.
class Validator{
	public:
		/// @brief The constructor. It takes an SSCFLSO instance and computes the rankings.
		/// @param instance defined in SSCFLSO.h
		Validator(const SSCFLSO&);
		/// @brief Sets the solution, computes the unique assignment, and value.
		/// @param open_facilities A container with those facilities that should be open.
		void set_solution(const std::vector<int>&);
		/// @brief Getter for the assignment.
		/// @return Client i is assigned to facility j iff assignment[i] = j.
		std::vector<int> get_assignment();
		/// @brief The objective value of the solution (opening costs + dist costs).
		/// @return The objective value.
		float value();
		/// @brief Checks if the solution is feasible (i.e. if no capacity limit is exceeded).
		/// @return True if the solution is feasible.
		bool feasible();
		/// @brief Computes the witness of feasibility. The solution is feasible iff the witness is empty.
		/// @return The facilities that have their capacity limit exceeded.
		std::vector<int> violating_facilities();
	private:
		const SSCFLSO& ref_instance;
		// Let f = preferences[i][k] for client i and some index k. Then rankings[i][f] = k.
		std::vector<std::vector<int>> rankings; 
		std::vector<int> open_facilities;
		float solution_value;
		//Index = Facility, Value = Client
		std::vector<int> assignment; 
		// Computes the rankings using the preferences.
		void assign_rankings();
};
#endif