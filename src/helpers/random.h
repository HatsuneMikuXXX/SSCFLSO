#include <random>
#include <time.h>
#include <assert.h>

float random();
float uniform(float lower_bound, float upper_bound, bool integer = false);
float triangular(float lower_bound, float upper_bound, float peak);