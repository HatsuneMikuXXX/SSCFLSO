#ifndef RANDOMIZED_RESTART_INFO_H
#define RANDOMIZED_RESTART_INFO_H

const std::string randomized_restart_info = R"~~~~(
Algorithm Information
Name: Randomized Restart
Idea: 
	Construct randomized solution after preprocessing and run local search first on them until local optima is reached.
	If no feasible initial solution is found after an amount of steps increase the success rate for opening a facility.
	If multiple feasible initial solutions are found in succession decrease the success rate for opening a facility.
	Then repeat this process until timeout is reached.
Parameters: None.
### END OF INFORMATION ###
)~~~~";

#endif