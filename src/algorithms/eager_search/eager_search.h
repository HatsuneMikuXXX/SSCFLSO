#ifndef EAGER_SEARCH_H
#define EAGER_SEARCH_H
#include "../algorithmClass.h"
#include "../local_search/local_search.h"

class EagerSearch : public Algorithm {
public:
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
};
#endif