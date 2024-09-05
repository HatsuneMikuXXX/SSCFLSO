#ifndef HELP_MSG_H
#define HELP_MSG_H

const std::string help_msg = R"~~~~(
usage: SSCFLSO (name of the executable) 
	[--help] 
	[--algos] (displays list of algorithms) 
	[--format] (displays specified format for SSCFLSO instances)
	[--gurobi] (will run gurobi after the algorithm finishes)
	--run input_file_or_directory output_directory time_limit_in_ms algorithms !!: Slurm branch: Input any timelimit. It won't be really considered.
)~~~~";
#endif