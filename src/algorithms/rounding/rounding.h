#ifndef ROUNDING_H
#define ROUNDING_H
#include "../algorithmClass.h"
#include "../../SSCFLSO/SSCFLSO_generator.h"
class Rounding : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit);
	std::string meta_information();
};
#endif