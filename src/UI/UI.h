#ifndef UI_H
#define UI_H

#include "../common.h"
#include "../libraries.h"
#include "../helpers/benchmark.h"
// Messages
#include "helpMessage.h"
#include "invalidCommandMessage.h"
#include "showAlgos.h"
#include "showFormat.h"


const int number_of_algos = 8;
const char* valid_algorithms[number_of_algos] = { "gurobi", "preprocess", "localsearch", "lsf", "greedy", "greedypop", "rounding", "randomizedrestart" }; // must be lower case
const TOKEN algo_tokens[number_of_algos] = { GUROBI_ALGO, PREPROCESS_ALGO, LOCAL_SEARCH_ALGO, GREEDY_ALGO, ROUNDING_ALGO, RANDOMIZED_RESTART_ALGO, };
const int number_of_flags = 5;
const char* valid_flags[number_of_flags] = { "help", "algos", "format", "run", "gurobi" };
const TOKEN flag_tokens[number_of_flags] = { HELP_FLAG, SHOW_ALGOS_FLAG, SHOW_FORMAT_FLAG, RUN_FLAG, GUROBI_AFTERWARDS_FLAG };

void start_UI(int argc, char* argv[]);
Algorithm* algorithmFactory(TOKEN algoToken);

const enum TOKEN {
	NO_TOKEN,
	INVALID,

	HELP_FLAG,
	SHOW_FORMAT_FLAG,
	SHOW_ALGOS_FLAG,
	GUROBI_AFTERWARDS_FLAG,
	RUN_FLAG,

	GUROBI_ALGO,
	PREPROCESS_ALGO,
	LOCAL_SEARCH_ALGO,
	GREEDY_ALGO,
	ROUNDING_ALGO,
	RANDOMIZED_RESTART_ALGO,

	INPUT_FILE_PATH,
	INPUT_DIRECTORY_PATH,
	OUTPUT_DIRECTORY_PATH,
	TIMELIMIT,
};

TOKEN scan_arg(const TOKEN& last_token, char* argument);

#endif