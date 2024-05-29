#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "common.h"
/**
 * Floating point arithmetic is generally not precise.
 * We therefore assume that two floating point numbers `a` and `b` are equal if
 * |`a` - `b`| < `EPSILON`.
*/
const double EPSILON = 10e-7;
const double GUROBI_TIME_BUFFER = 90000;
const double GUROBI_BUFFER = 2;
const std::chrono::milliseconds TWO_MINUTES(120 * 1000);
#endif