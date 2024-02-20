#include "rounding.h"

facility_vector rounding(const SSCFLSO& instance){
	Validator FLV = Validator(instance);
	facility_vector solution = facility_vector(instance.facilities, 0);
	relaxed_solution rs = solve(instance);
	// Round up
	for (int i = 0; i < instance.facilities; i++) {
		if (rs.rFacility_vector[i] > 0) {
			solution[i] = 1;
		}
	}
	// Close empty facilities
	FLV.set_solution(solution);
	FLV.drop_empty_facilities();
	assert(FLV.feasible());
	return solution;
}