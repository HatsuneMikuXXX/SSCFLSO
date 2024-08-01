#ifndef UNIT_TEST_UTILITY_H
#define UNIT_TEST_UTILITY_H
#include "algorithmClass.h"

void expect_solution(Algorithm* algo, const SSCFLSO& instance, const int time_limit, const facility_vector& expected_solution);
void check_timelimit_enforcement(Algorithm* algo, const SSCFLSO& instance); 
void print_current_system_time();

#endif
