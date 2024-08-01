#ifndef UI_H
#define UI_H

#include "../common.h"
#include "../libraries.h"
#include "../benchmarking/benchmark.h"
#include "../../third party libraries/dirent/include/dirent.h"
// Messages
#include "helpMessage.h"
#include "invalidCommandMessage.h"
#include "showAlgos.h"
#include "showFormat.h"

const enum TOKEN {
	NO_TOKEN,
	INVALID,

	HELP_FLAG,
	SHOW_FORMAT_FLAG,
	SHOW_ALGOS_FLAG,
	GUROBI_AFTERWARDS_FLAG,
	RUN_FLAG,

	COMPOSITE_ALGO,
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

	NEXTNEIGHBOR_BEST_PARAM,
	NEXTNEIGHBOR_FIRST_PARAM,
	SOLBY_PREPROCESS_PARAM,
	SOLBY_RANDOM_RESTART_PARAM,
	SOLBY_RANDOM_PARAM,
	SOLBY_GIVEN_PARAM,

	INPUT_FILE_PATH,
	INPUT_DIRECTORY_PATH,
	OUTPUT_DIRECTORY_PATH,
	TIMELIMIT,
};

const int number_of_algos = 11;
const std::string valid_algorithms[number_of_algos] = { "composite", "grasp", "greedy", "gurobi", "lagrangianrelaxation", "localsearch", "preprocess", "rounding", "semilagrangianrelaxation", "simulatedannealing", "tabusearch"}; // must be lower case
const TOKEN algo_tokens[number_of_algos] = { COMPOSITE_ALGO, GRASP_ALGO, GREEDY_ALGO, GUROBI_ALGO, LAGRANGIAN_RELAXATION_ALGO, LOCAL_SEARCH_ALGO, PREPROCESS_ALGO, ROUNDING_ALGO, SEMI_LAGRANGIAN_RELAXATION_ALGO, SIMULATED_ANNEALING_ALGO, TABU_SEARCH_ALGO, };
const int number_of_flags = 5;
const std::string valid_flags[number_of_flags] = { "help", "algos", "format", "run", "gurobi" };
const TOKEN flag_tokens[number_of_flags] = { HELP_FLAG, SHOW_ALGOS_FLAG, SHOW_FORMAT_FLAG, RUN_FLAG, GUROBI_AFTERWARDS_FLAG };
const int number_of_init_params = 4;
const std::string valid_init_params[number_of_init_params] = {"preprocess", "randomrestart", "random", "given"};
const TOKEN init_param_tokens[number_of_init_params] = {  SOLBY_PREPROCESS_PARAM, SOLBY_RANDOM_RESTART_PARAM, SOLBY_RANDOM_PARAM, SOLBY_GIVEN_PARAM };
const int number_of_next_params = 2;
const std::string valid_next_params[number_of_next_params] = {"best", "first"};
const TOKEN next_param_tokens[number_of_next_params] = { NEXTNEIGHBOR_BEST_PARAM, NEXTNEIGHBOR_FIRST_PARAM };

void start_UI(int argc, char* argv[]);
TOKEN scan_arg(std::vector<TOKEN>& stack, char* argument);


Algorithm* algorithmFactory(TOKEN* token_string, int& current_index, int max_index);


void execute_run_command(
	const bool inputSourceIsDirectory,
	const std::string& inputSource,
	const std::string& outputTarget,
	const int timelimit,
	Algorithm* const algoObjects[number_of_algos], 
	const int algoObjectsSize,
	const bool runAlgoWithGurobi);

#endif