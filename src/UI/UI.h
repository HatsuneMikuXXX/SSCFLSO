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

const int number_of_algos = 10;
const std::string valid_algorithms[number_of_algos] = { "grasp", "greedy", "gurobi", "lagrangianrelaxation", "localsearch", "preprocess", "rounding", "semilagrangianrelaxation", "simulatedannealing", "tabusearch"}; // must be lower case
const TOKEN algo_tokens[number_of_algos] = { GRASP_ALGO, GREEDY_ALGO, GUROBI_ALGO, LAGRANGIAN_RELAXATION_ALGO, LOCAL_SEARCH_ALGO, PREPROCESS_ALGO, ROUNDING_ALGO, SEMI_LAGRANGIAN_RELAXATION_ALGO, SIMULATED_ANNEALING_ALGO, TABU_SEARCH_ALGO, };
const int number_of_flags = 5;
const std::string valid_flags[number_of_flags] = { "help", "algos", "format", "run", "gurobi" };
const TOKEN flag_tokens[number_of_flags] = { HELP_FLAG, SHOW_ALGOS_FLAG, SHOW_FORMAT_FLAG, RUN_FLAG, GUROBI_AFTERWARDS_FLAG };

const enum TOKEN {
	NO_TOKEN,
	INVALID,

	HELP_FLAG,
	SHOW_FORMAT_FLAG,
	SHOW_ALGOS_FLAG,
	GUROBI_AFTERWARDS_FLAG,
	RUN_FLAG,

	GRASP_ALGO,
	GREEDY_ALGO,
	GUROBI_ALGO,
	LAGRANGIAN_RELAXATION_ALGO,
	LOCAL_SEARCH_ALGO,
	PREPROCESS_ALGO,
	ROUNDING_ALGO,
	SEMI_LAGRANGIAN_RELAXATION_ALGO,
	SIMULATED_ANNEALING_ALGO,
	TABU_SEARCH_ALGO,
	
	SOLBY_PREPROCESS_PARAM,
	SOLBY_RANDOM_RESTART_PARAM,
	SOLBY_RANDOM_PARAM,
	SOLBY_GIVEN_PARAM,

	INPUT_FILE_PATH,
	INPUT_DIRECTORY_PATH,
	OUTPUT_DIRECTORY_PATH,
	TIMELIMIT,
};

void start_UI(int argc, char* argv[]);
TOKEN scan_arg(const TOKEN& last_token, char* argument);


Algorithm* algorithmFactory(TOKEN algoToken);


void execute_run_command(
	const bool inputSourceIsDirectory,
	const std::string& inputSource,
	const std::string& outputTarget,
	const int timelimit,
	Algorithm* const algoObjects[number_of_algos], 
	const int algoObjectsSize,
	const bool runAlgoWithGurobi);

#endif