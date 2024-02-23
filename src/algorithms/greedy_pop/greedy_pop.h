#ifndef GREEDY_POP_H
#define GREEDY_POP_H
#include "../algorithmClass.h"
/**
 * Utility is computed in the following way:
 * The utility of unnecessary facilities is always 0. The remaining description skips unnecessary facilities:
 * For a facility j, adjust the distribution cost to client i with a factor x in [0, 1]
 * x = 0 if and only if the cost is never realized (a feasible solution must have at least k facilities, thus every facility j
 * that is worse than the k-th facility of client i is never assigned to i)
 * x = 1 if and only if the facility is the most preferred
 * Then, x linearly decreases.
 * The utility is thus: capacity_j / [ facility_cost_j + (sum_I adjusted distribution costs) ]
*/
class GreedyPop : public Algorithm {
public:
	static void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit);
	static std::string meta_information();
private:
	static std::vector<std::pair<int, double>> utility(const SSCFLSO& instance, const facility_vector& no_unnecessary_facilities, bool popularity);
};
facility_vector greedy_with_popularity(const SSCFLSO& instance);
facility_vector greedy_without_popularity(const SSCFLSO& instance);
#endif