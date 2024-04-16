#ifndef ROUNDING_H
#define ROUNDING_H
#include "../algorithmClass.h"
#include "../../SSCFLSO/SSCFLSO_generator.h"
#include "roundingInfo.h"
class Rounding : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards);
	std::string meta_information();
};
#endif