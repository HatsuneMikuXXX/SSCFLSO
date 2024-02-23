#include "random.h"
double random(){
	return double(rand())/RAND_MAX;
}

double uniform(double lower_bound, double upper_bound, bool integer){
	double r = random();
	if(!integer){
		return r * lower_bound + (1. - r) * upper_bound;
	}
	int new_lb = ceil(lower_bound);
	int new_ub = floor(upper_bound);
	double x = floor(r * new_lb + (1. - r) * (new_ub + 1)); // (ub + 1) because we round down.
	if(x == new_ub + 1){
		// This case 'almost never' occurs, i.e. (should) have a chance of 0.
		return new_ub;
	}
	return x;
}

double triangular(double lower_bound, double upper_bound, double peak){
	if(peak < lower_bound || peak > upper_bound){
		throw std::runtime_error("Peak must be defined within the bounds.");
	}
	double u = uniform(lower_bound, upper_bound);
	double v = uniform(lower_bound, upper_bound);
	double c = (peak - lower_bound)/(upper_bound - lower_bound);
	auto min = [](double a, double b) { return a < b ? a : b; };
	auto max = [](double a, double b) { return a > b ? a : b; };
	return (1. - c) * min(u, v) + c * max(u, v);
}