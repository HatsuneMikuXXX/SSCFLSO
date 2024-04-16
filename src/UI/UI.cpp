#include "UI.h"

void UI_invalid_msg() {
	std::cout << "Flag not recognized. Use the -help or --help flag for more information about the usage of this program." << std::endl;
}

void UI_help_msg() {
	std::cout << "Flags:\n\t-run/--run\t\t[Relative path to plc file] [Relative path to save results to] [Algorithm name] [Time limit in milliseconds]" << std::endl;
}

int stringFlagtoIntCode(const char* flag) {
	const int number_of_flags = 3;
	const char* valid_flags[number_of_flags] = { "help", "run", "gurobi" };
	for (int i = 0; i < number_of_flags; i++) {
		std::string tmp1 = "-";
		std::string tmp2 = "-";
		tmp1 += valid_flags[i];
		tmp2 += tmp1;
		if (strcmp(flag, tmp1.c_str()) == 0 || strcmp(flag, tmp2.c_str()) == 0) {
			return i;
		}
	}
	return -1;
}

int stringAlgotoIntCode(const char* flag) {
	const int number_of_algos = 7;
	const char* valid_algorithms[number_of_algos] = { "gurobi", "preprocess", "localsearch", "lsf", "greedy", "greedypop", "rounding" }; // must be lower case

	std::string flag_as_string = std::string(flag);
	std::transform(flag_as_string.begin(), flag_as_string.end(), flag_as_string.begin(), [](unsigned char c) { return std::tolower(c); }); // To lower case
	for (int i = 0; i < number_of_algos; i++) {
		if (strcmp(flag_as_string.c_str(), valid_algorithms[i]) == 0) {
			return i;
		}
	}
}

Algorithm* algorithmFactory(int algorithmIntCode) {
	switch (algorithmIntCode) {
	case GUROBI:
		return new Gurobi();
	case PREPROCESS:
		return new Preprocess();
	case LOCALSEARCH:
		return new LocalSearch();
	case LSF:
		return new LocalSearchFirst();
	case GREEDY:
		return new Greedy();
	case GREEDYPOP:
		return new GreedyPop();
	case ROUNDING:
		return new Rounding();
	default:
		throw std::runtime_error("Algorithm code not recognized.");
	}
}


