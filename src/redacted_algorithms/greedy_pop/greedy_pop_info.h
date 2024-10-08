/*
#ifndef GREEDY_POP_INFO_H
#define GREEDY_POP_INFO_H

const std::string greedy_pop_info = R"~~~~(
Algorithm Information
Name: Greedy Pop
Idea: 
	Utility is computed slightly differently as in the classical Greedy algorithm.
	Each distribution cost is scaled by a popularity factor and then the utility is computed as usual
	(capacity divided by facility cost plus the sum of adjusted distribution costs).
	The formula for the popularity factor requires certain numbers:
	- m_u := Number of non-unnecessary facilities
	- m_l := Smallest number of facilities required so that their whole capacity covers the sum of demands
	- r_ij := Rank of facility j of client i among the non-unnecessary facilities (zero-based).
	  Rank 0 corresponds to the most preferred facility that is not unnecessary.
	The popularity factor for the distribution cost of facility j to client i is then:
				
		= 1 - (r_ij / (m_u - m_l + 1)), if r_ij LEQ m_u - m_l + 1
		= 0, otherwise

	Add iteratively those facilities that give us the greatest utility gain.
	Stop as soon as we have a feasible solution.
	(Non-Greedy typical) Finally, remove all non-serving facilities.
Parameters: None.
### END OF INFORMATION ###
)~~~~";

#endif
*/