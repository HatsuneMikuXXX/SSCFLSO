#ifndef PREPROCESS_H
#define PREPROCESS_H
#include "../algorithmClass.h"
#include "preprocessInfo.h"
class Preprocess : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards);
	std::string meta_information();
};
#endif