#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H
#include "../algorithmClass.h"
#include "../Preprocess/preprocess.h"
#include"../../helpers/random.h"
#include "../local_search/local_search.h"

class SimulatedAnnealing : public Algorithm {
public:
	enum INITIAL_SOLUTION {
		PREPROCESS,
		RANDOM_RESTART,
		RANDOM,
		GIVEN,
	};
	SimulatedAnnealing(INITIAL_SOLUTION init);
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
private:
	const INITIAL_SOLUTION init;
};

facility_vector get_random_neighbor(int max_hamming_distance, const facility_vector& not_unnecessary, facility_vector& solution);


#endif
