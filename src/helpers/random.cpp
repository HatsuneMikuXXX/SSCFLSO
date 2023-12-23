#include "random.h"
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

float triangular(float lower_bound, float upper_bound, float peak){
	if(peak < lower_bound || peak > upper_bound){
		throw std::runtime_error("Peak must be defined within the bounds.");
	}
	float u = uniform(lower_bound, upper_bound);
	float v = uniform(lower_bound, upper_bound);
	float c = (peak - lower_bound)/(upper_bound - lower_bound);
	auto min = [](float a, float b) { return a < b ? a : b; };
	auto max = [](float a, float b) { return a > b ? a : b; };
	return (1. - c) * min(u, v) + c * max(u, v);
}