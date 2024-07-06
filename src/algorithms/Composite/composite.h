#ifndef COMPOSITE_H
#define COMPOSITE_H
#include "../algorithmClass.h"

class Composite : public Algorithm {
public:
	Composite()
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
private:
	std::vector<Algorithm> algorithms;
};
#endif