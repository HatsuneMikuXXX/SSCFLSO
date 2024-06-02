#include "random.h"
double random(){
	return double(rand())/RAND_MAX;
}

bool flip() {
	return random() > 0.5;
}

bool biased_flip(const double p) {
	return random() < p;
}

double uniform(const double lower_bound, const double upper_bound, const bool integer){
	if (!integer) {
		const double r = random();
		return r * lower_bound + (1. - r) * upper_bound;
	}
	else {
		const double r = random();
		const int new_lb = ceil(lower_bound);
		const int new_ub = floor(upper_bound) + 1; //We round down afterwards.
		const double floating_point_sample = r * new_lb + (1. - r) * new_ub;
		const int integer_point_sample = floor(floating_point_sample);
		if (integer_point_sample == new_ub) {
			// Extra fail-safe. Probability of this occuring is almost never happening.
			return new_ub - 1;
		}
		return integer_point_sample;
	}
}

double triangular(const double lower_bound, const double upper_bound, const double peak){
	if(peak < lower_bound || peak > upper_bound){
		throw std::runtime_error("Peak must be defined within the bounds.");
	}
	const double u = uniform(lower_bound, upper_bound);
	const double v = uniform(lower_bound, upper_bound);
	const double min = (u < v) ? u : v;
	const double max = (u > v) ? u : v;
	const double c = (peak - lower_bound)/(upper_bound - lower_bound);
	return (1. - c) * min + c * max;
}