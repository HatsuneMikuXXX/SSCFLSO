#ifndef ROUNDING_INFO_H
#define ROUNDING_INFO_H

const std::string rounding_info = R"~~~~(
Algorithm Information
Name: Rounding
Idea: 
	Solve the linear relaxation of the SSCFLSO instance.
	For every facility that is open partially, open it completely (aka round everything up).
	This guarantees us a feasible solution.
Parameters: None.
### END OF INFORMATION ###
)~~~~";

#endif