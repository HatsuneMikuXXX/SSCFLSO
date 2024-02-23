#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H
#include "../algorithmClass.h"
/// Strategy: Take best feasible neighbor found. 
class LocalSearch : public Algorithm {
public:
	static void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit);
	static std::string meta_information();
};
#endif