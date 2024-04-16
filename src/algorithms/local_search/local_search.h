#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H
#include "../algorithmClass.h"
#include "local_search_info.h"
/// Strategy: Take best feasible neighbor found. 
class LocalSearch : public Algorithm {
public:
	void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit, const bool gurobi_afterwards);
	std::string meta_information();
};
#endif