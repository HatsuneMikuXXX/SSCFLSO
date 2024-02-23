#ifndef HELPER_H
#define HELPER_H
#include "../common.h"
#include "../constants.h"
#include "../typedefinitions.h"
#include "data_helper_impl.h"

std::function<bool(const std::pair<int, double>&, const std::pair<int, double>&)> sort_by_first = [](const std::pair<int, double>& pair1, const std::pair<int, double>& pair2) {
	return (pair1.first < pair2.first);
};

std::function<bool(const std::pair<int, double>&, const std::pair<int, double>&)> sort_by_second = [](const std::pair<int, double>& pair1, const std::pair<int, double>& pair2) {
	return (pair1.second < pair2.second);
};

/// Inverse index set, example: [3,0,1,4,5,2,6] becomes [1,2,5,0,3,4,6]
std::vector<int> inverse(const std::vector<int>& index_set);

void filter(facility_vector& facilities, const facility_predicate& predicate);

bool areSame(double a, double b);
bool areSame(const std::vector<double>& a, const std::vector<double>& b);
bool areSame(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b);

int sum(const std::vector<int>& a);
double sum(const std::vector<double>& a);

int hamming_distance(const facility_vector& a, const facility_vector& b);
// Returns ||v||_2
double magnitude(const std::vector<double>& v);

// Generic functions
template<typename T>
bool contains(const std::vector<T>& container, const T& x);

template<typename T1, typename T2>
std::vector<T1> projection_1_2(const std::vector<std::pair<T1, T2>>& pairs);

template<typename T>
void bisect_insert(std::vector<T>& container, const T& x, const std::function<bool(const T&, const T&)>& comes_before);
#endif