#include "rounding.h"

class Rounding : public Algorithm {
public:
	static void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit) {
		Validator FLV = Validator(instance);
		facility_vector solution = facility_vector(instance.facilities, 0);
		relaxed_solution rs = solve_linear_relaxation(instance);
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
		current_best = solution;
	}

	static std::string meta_information() {
		std::ifstream file(PATH + "rounding/rounding.txt");
		if (file) {
			std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
			return content;
		}
		else {
			throw std::runtime_error("Rounding information file not found.");
		}
	}
};