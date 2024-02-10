#ifndef VALIDATOR_H
#define VALIDATOR_H
#include "../common.h"
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
		void set_solution(const facility_vector&);
		/// @brief Get the solution
		facility_vector get_solution();
		/// @brief Getter for the assignment.
		/// @return Client i is assigned to facility j iff assignment[i] = j.
		client_facility_assignment get_assignment();
		/// @brief The objective value of the solution (opening costs + dist costs).
		/// @return The objective value.
		double value() const;
		/// @brief Checks if the solution is feasible (i.e. if no capacity limit is exceeded).
		/// @return True if the solution is feasible.
		bool feasible();
		/// @brief Computes the witness of feasibility. The solution is feasible iff the witness is empty.
		/// @return The facilities that have their capacity limit exceeded.
		facility_vector exceeds_capacity();
		///  @brief Remove facilities that serve no one.
		void drop_empty_facilities();
	private:
		const SSCFLSO& ref_instance;
		// Let f = preferences[i][k] for client i and some index k. Then rankings[i][f] = k.
		std::vector<std::vector<int>> rankings; 
		// Binary vector
		facility_vector solution;
		double solution_value;
		//Index = Facility, Value = Client
		client_facility_assignment assignment;
};
#endif