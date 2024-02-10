#include "preprocess.h"

/**
 * Removes unnecessary facilities which are:
 * - Type I: Facilities that when opened always cause the solution to be infeasible
 * - Type II: Facilities that cannot even serve one client
*/

facility_vector preprocess(const SSCFLSO& instance){
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
	facility_predicate predicate = [capacities, minimum_demand](int facility){ return capacities[facility] >= minimum_demand; };
	filter(no_unnecessary_facilities, predicate);
	return no_unnecessary_facilities;
}