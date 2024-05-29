#ifndef HELP_MSG_H
#define HELP_MSG_H

const std::string help_msg = R"~~~~(
Flags: -run/--run 
	Parameters: Relative path to plc file or directory, Relative path to directory to save results to, Time limit in milliseconds, Any number of algorithms

	!!Notice!!: .plc files have a specific format. To see the format, use the command "-show format" or "--show format"

	1): If a directory is given, all plc files are scanned. If a plc file is not in the correct format, we cancel the command.
	2): Results are saved with the name "Instance name"_results.json
	3): Time limit. We expect a whole number.
	4): Any number of valid algorithms can be given. The names for the valid algorithms can be opened using the command "-show algorithms" or "-show algorithms"
)~~~~";
#endif