#ifndef LOCAL_SEARCH_INFO_H
#define LOCAL_SEARCH_INFO_H

const std::string local_search_info = R"~~~~(
Algorithm Information
Name: Local Search
Idea: 
	Consider the Remove, Swap, and Add (RSA) neighborhood of a solution/binary vector.
	Let x and y be solutions then define HD(x, y) as the hamming distance of x and y.
	The neighborhoods of a solution x are defined as follows:
	(A): {y | HD(x, y) = 1 & Exists j: y[j] = 1 & x[j] = 0} // Deprecated. Not required.
	(R): {y | HD(x, y) = 1 & Exists j: y[j] = 0 & x[j] = 1}
	(S): {y | HD(x, y) = 2 & Exists j, k: y[j] = 1 & x[j] = 0 & y[k] = 0 & x[k] = 1}
	Search those neighborhoods and consider that solution that decreases the objective function the most.
	Repeat until no further improvement can be done.
Parameters: None.
### END OF INFORMATION ###
)~~~~";

#endif