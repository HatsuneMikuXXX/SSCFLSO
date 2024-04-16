#ifndef GREEDY_INFO_H
#define GREEDY_INFO_H

const std::string greedy_info = R"~~~~(
Algorithm Information
Name: Greedy
Idea: 
	Compute utility of each (non-unnecessary) facility as their capacity divided by their facility cost and sum of all their distribution costs.
	Add iteratively those facilities that give us the greatest utility gain.
	Stop as soon as we have a feasible solution.
	(Non-Greedy typical) Finally, remove all non-serving facilities.
Parameters: None.
### END OF INFORMATION ###
)~~~~";

#endif