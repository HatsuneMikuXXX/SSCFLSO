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
	};
	enum NEXT_NEIGHBOR {
		BEST,
		FIRST,
	};
	LocalSearch(INITIAL_SOLUTION init, NEXT_NEIGHBOR next);
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
private:
	const INITIAL_SOLUTION init;
	const NEXT_NEIGHBOR next;
	facility_vector produce_initial_solution(const SSCFLSO& instance, Validator& FLV, Timer& timer, ReportResult& report);
	void get_next_neighbor(Validator& FLV, facility_vector& solution, NEXT_NEIGHBOR nn_code);
};
#endif