#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "common.h"
/**
 * Floating point arithmetic is generally not precise.
 * We therefore assume that two floating point numbers `a` and `b` are equal if
 * |`a` - `b`| < `EPSILON`.
*/
const double EPSILON = 10e-7;
const int ONE_MINUTE = 60000;
#endif