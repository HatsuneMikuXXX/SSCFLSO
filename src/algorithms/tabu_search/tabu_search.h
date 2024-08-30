#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H
#include "../algorithmClass.h"
#include "../local_search/local_search.h"
#include "../preprocess/preprocess.h"

class TabuSearch : public Algorithm {
public:
	enum INITIAL_SOLUTION {
		PREPROCESS,
		RANDOM_RESTART,
		RANDOM,
		GIVEN,
	};
	TabuSearch(INITIAL_SOLUTION init);
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
private:
	const INITIAL_SOLUTION init{PREPROCESS};
};
#endif