#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H
#include "../algorithmClass.h"
#include "../Preprocess/preprocess.h"
#include "../../helpers/random.h"

class LocalSearch : public Algorithm {
public:
	enum INITIAL_SOLUTION {
		PREPROCESS,
		RANDOM_RESTART,
		RANDOM,
		GIVEN,
	};
	enum NEXT_NEIGHBOR {
		BEST = 200,
		FIRST = 201,
	};
	LocalSearch();
	LocalSearch(INITIAL_SOLUTION init);
	LocalSearch(INITIAL_SOLUTION init, NEXT_NEIGHBOR next);
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
	bool get_next_neighbor(Validator& FLV, facility_vector& solution) const;
	facility_vector produce_initial_solution(const SSCFLSO& instance, Validator& FLV, Timer& timer, ReportResult& report) const;
private:
	const INITIAL_SOLUTION init{PREPROCESS};
	const NEXT_NEIGHBOR next{BEST};
};
bool attempt_to_find_feasible_solution(facility_vector& initial_solution, Validator& FLV, int maxIter = 10e5);
#endif