#ifndef RANDOM_H
#define RANDOM_H
#include "../common.h"
#include "../helpers/utility.h"
// Return a random value from [0, 1]
double random();
// Return a uniformly-drawn, random value from {0, 1}
bool flip();
// Return '1' with a probability p and '0' with a probability 1 - p
bool biased_flip(const double p);
// Return a random value from [lower_bound, upper_bound]. If [param: integer] is true then return a random value from {ceil(lower_bound),...,floor(upper_bound)} instead. 
double uniform(const double lower_bound, const double upper_bound, const bool integer = false);
/// Draw from a triangular distribution. See MINMAX method from https://www.sciencedirect.com/science/article/pii/S0895717708002665
double triangular(const double lower_bound, const double upper_bound, const double peak);
#endif