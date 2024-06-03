#ifndef HELPER_H
#define HELPER_H
#include "../common.h"
#include "../constants.h"
#include "../typedefinitions.h"

template<typename T>
struct alreadyComputed {
	bool aCom = false;
	T value;
};

void filter(facility_vector& facilities, const facility_predicate& predicate);

bool are_same(double a, double b);
bool are_same(const std::vector<double>& a, const std::vector<double>& b);
bool are_same(const std::vector<int>& a, const std::vector<int>& b);
bool are_same(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b);

int sum(const std::vector<int>& a);
double sum(const std::vector<double>& a);

int hamming_distance(const facility_vector& a, const facility_vector& b);
// Returns ||v||_2
double magnitude(const std::vector<double>& v);

std::string primitive_list_to_string(const std::vector<int>& container);
std::string primitive_list_to_string(const std::vector<double>& container);

#include "utility_impl.h"
#endif