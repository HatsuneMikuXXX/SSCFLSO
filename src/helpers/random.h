#ifndef RANDOM_H
#define RANDOM_H
#include "../common.h"
#include "../helpers/data_helper.h"
/// Get a random value in [0, 1]
float random();
/// @brief Draw uniformly a random number in [lower_bound, upper_bound]
/// @param lower_bound the lower bound
/// @param upper_bound the upper bound
/// @param integer if true then draw uniformly a random number from [lower_bound, upper_bound] intersected with the integers
/// @return a random number
float uniform(float lower_bound, float upper_bound, bool integer = false);
/// Draw from a triangular distribution. It must hold lower_bound LEQ peak LEQ upper_bound. See MINMAX method from https://www.sciencedirect.com/science/article/pii/S0895717708002665
float triangular(float lower_bound, float upper_bound, float peak);
#endif