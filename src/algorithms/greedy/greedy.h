#ifndef GREEDY_H
#define GREEDY_H
#include "../algorithm.h"
/**
 * If preprocessing returns a feasible solution set S.
 * Greedy starts with the empty solution.
 * Iteratively add those facilities of S to the solution
 * that create the lowest cost. Stop as soon as we have a feasible solution
*/
solution greedy(const SSCFLSO& instance);
#endif