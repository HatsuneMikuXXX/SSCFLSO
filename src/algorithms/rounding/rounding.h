#ifndef ROUNDING_H
#define ROUNDING_H
#include "../algorithmClass.h"
#include "../../SSCFLSO/SSCFLSO_generator.h"
#include "../Preprocess/preprocess.h"
class Rounding : public Algorithm {
public:
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
};
#endif