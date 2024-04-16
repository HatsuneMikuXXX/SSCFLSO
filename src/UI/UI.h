#ifndef UI_H
#define UI_H

#include "../common.h"
#include "../algorithms/algorithmClass.h"
#include "../algorithms/local_search/local_search.h"
#include "../algorithms/local_search_first/local_search_first.h"
#include "../algorithms/greedy/greedy.h"
#include "../algorithms/greedy_pop/greedy_pop.h"
#include "../algorithms/rounding/rounding.h"
#include "../algorithms/gurobi/gurobi.h"
#include "../algorithms/preprocess/preprocess.h"

void UI_invalid_msg();
void UI_help_msg();
int stringFlagtoIntCode(const char* flag);
int stringAlgotoIntCode(const char* flag);
Algorithm* algorithmFactory(int algorithmIntCode);

const enum FLAGS {
	HELP = 0,
	RUN = 1,
	USE_GUROBI_AFTERWARDS = 2,
}; // If you update this, remember to update the UI.cpp file accordingly!!


const enum ALGORITHMS {
	GUROBI = 0,
	PREPROCESS = 1,
	LOCALSEARCH = 2,
	LSF = 3,
	GREEDY = 4,
	GREEDYPOP = 5,
	ROUNDING = 6,
}; // If you update this, remember to update the UI.cpp file accordingly!!

#endif