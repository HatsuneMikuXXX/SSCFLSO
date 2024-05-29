#include "random.h"
double random(){
	return double(rand())/RAND_MAX;
}

bool flip() {
	return random() > 0.5;
}

bool biased_flip(double p) {
	return random() < p;
}

double uniform(double lower_bound, double upper_bound, bool integer){
	if (!integer) {
		double r = random();
		return r * lower_bound + (1. - r) * upper_bound;
	}
	else {
		double r = random();
		int new_lb = ceil(lower_bound);
		int new_ub = floor(upper_bound) + 1; //We round down afterwards.
		double floating_point_sample = r * new_lb + (1. - r) * new_ub;
		int integer_point_sample = floor(floating_point_sample);
		if (integer_point_sample == new_ub) {
			// Extra fail-safe. Probability of this occuring is almost never happening.
			return new_ub - 1;
		}
		return integer_point_sample;
	}
}

double triangular(double lower_bound, double upper_bound, double peak){
	if(peak < lower_bound || peak > upper_bound){
		throw std::runtime_error("Peak must be defined within the bounds.");
	}
	double u = uniform(lower_bound, upper_bound);
	double v = uniform(lower_bound, upper_bound);
	double min = (u < v) ? u : v;
	double max = (u > v) ? u : v;
	double c = (peak - lower_bound)/(upper_bound - lower_bound);
	return (1. - c) * min + c * max;
}