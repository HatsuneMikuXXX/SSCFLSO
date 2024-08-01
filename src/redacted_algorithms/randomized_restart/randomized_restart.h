/*
#ifndef RANDOMIZED_RESTART_H
#define RANDOMIZED_RESTART_H
#include "../algorithmClass.h"
#include "randomized_restart_info.h"
class RandomizedRestart : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards);
	std::string meta_information();
private:
	bool rr_contains(const std::vector<facility_vector>& container, const facility_vector& x);
};
#endif
*/