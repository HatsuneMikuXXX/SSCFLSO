#ifndef DONOTHING_H
#define DONOTHING_H
#include "../algorithmClass.h"
class DoNothing : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit);
	std::string meta_information();
};
#endif