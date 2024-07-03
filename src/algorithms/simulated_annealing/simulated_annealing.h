#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H
#include "../algorithmClass.h"
#include "../Preprocess/preprocess.h"
#include"../../helpers/random.h"

class SimulatedAnnealing : public Algorithm {
public:
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
};

facility_vector get_random_neighbor(int max_hamming_distance, const facility_vector& unnecessary, facility_vector& solution);


#endif
