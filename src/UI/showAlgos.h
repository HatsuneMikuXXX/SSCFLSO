#ifndef ALGOS_MSG_H
#define ALGOS_MSG_H

const std::string algos_msg = R"~~~~(
List of algorithm names (case-insensitive):
	preprocess
	localsearch best/first (refers to feasible neighbor) preprocess/given/random/randomrestart 
		(given: a preceding algorithm provides the solution. 
		 random: randomize a solution and use local search to find a feasible solution
		 randomrestart: randomize a solution until it's feasible.)
	greedy
	rounding
	grasp value (any fraction in [0,1])
	simulatedannealing preprocess/given/random/randomrestart (same as localsearch)
	tabusearch preprocess/given/random/randomrestart (same as localsearch)
	lagrangianrelaxation
	semilagrangianrelaxation value (true or false; if true then weights are updated homogenously)
	gurobi
	composite algo1 algo2 [...algos] (chain algorithms with the argument "given" together.) 
)~~~~";
#endif