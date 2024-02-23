#ifndef ROUNDING_H
#define ROUNDING_H
#include "../algorithmClass.h"
class Rounding : public Algorithm {
public:
	static void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit);
	static std::string meta_information();
};
#endif