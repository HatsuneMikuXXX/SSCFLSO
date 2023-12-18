#include "random.h"
using namespace std;

int SEED = 0; // Currently not in used but maybe for experiments later on.

float random(){
	return float(rand())/RAND_MAX;
}

float uniform(float lower_bound, float upper_bound, bool integer){
	float r = random();
	if(!integer){
		return r * lower_bound + (1. - r) * upper_bound;
	}
	int new_lb = ceil(lower_bound);
	int new_ub = floor(upper_bound);
	float x = floor(r * new_lb + (1. - r) * (new_ub + 1)); // (ub + 1) because we round down.
	if(x == new_ub + 1){
		// This case 'almost never' occurs, i.e. (should) have a chance of 0.
		return new_ub;
	}
	return x;
}

// Return a random number from a triangular distribution. MINMAX method from https://www.sciencedirect.com/science/article/pii/S0895717708002665
float triangular(float lower_bound, float upper_bound, float peak){
	assert(peak >= lower_bound && peak <= upper_bound);
	float u = uniform(lower_bound, upper_bound);
	float v = uniform(lower_bound, upper_bound);
	float c = (peak - lower_bound)/(upper_bound - lower_bound);
	return (1. - c) * min(u, v) + c * max(u, v);
}