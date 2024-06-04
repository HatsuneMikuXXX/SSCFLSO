#ifndef PREPROCESS_H
#define PREPROCESS_H
#include "../algorithmClass.h"

class Preprocess : public Algorithm {
public:
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards);
	std::string name();
};
#endif