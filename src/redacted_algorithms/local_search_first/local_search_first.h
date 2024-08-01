/*
#ifndef LOCAL_SEARCH_FIRST_H
#define LOCAL_SEARCH_FIRST_H
#include "../algorithmClass.h"
#include "local_search_first_info.h"
/// Strategy: Take first feasible neighbor found. 
class LocalSearchFirst : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards);
	std::string meta_information();
};
#endif
*/