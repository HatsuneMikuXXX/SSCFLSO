#ifndef COMPOSITE_H
#define COMPOSITE_H
#include "../algorithmClass.h"

class Composite : public Algorithm {
public:
	Composite(const std::vector<Algorithm*>& algorithms);
	void solve(const SSCFLSO& instance, solution_and_value& current_best, Timer& timer, ReportResult& report, const bool gurobi_afterwards) const;
	std::string name() const;
	bool post_applyable() const;
private:
	const std::vector<Algorithm*> algorithms;
};
#endif