#ifndef GRASP_H
#define GRASP_H
#include <climits>
#include "../algorithmClass.h"
#include "../preprocess/preprocess.h"
#include "../../helpers/random.h"
#include "../local_search/local_search.h"

class GRASP : public Algorithm {
public:
	GRASP();
	GRASP(double RCL_percentile);
	GRASP(int maxIter, double RCL_percentile);
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
private:
	const int maxIter{ INT_MAX };
	const double RCL_percentile{ 0.3 };
	void greedy_random(Validator& FLV, const facility_vector& CL, facility_vector& solution) const;
};
#endif