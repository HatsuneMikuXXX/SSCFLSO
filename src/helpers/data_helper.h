#ifndef HELPER_H
#define HELPER_H
#include "../common.h"
#include "../constants.h"
#include "../typedefinitions.h"

extern const std::function<bool(const std::pair<int, double>&, const std::pair<int, double>&)> sort_by_first;
extern const std::function<bool(const std::pair<int, double>&, const std::pair<int, double>&)> sort_by_second;

/// Inverse index set, example: [3,0,1,4,5,2,6] becomes [1,2,5,0,3,4,6]
std::vector<int> inverse(const std::vector<int>& index_set);

void filter(facility_vector& facilities, const facility_predicate& predicate);

bool areSame(double a, double b);
bool areSame(const std::vector<double>& a, const std::vector<double>& b);
bool areSame(const std::vector<int>& a, const std::vector<int>& b);
bool areSame(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b);

int sum(const std::vector<int>& a);
double sum(const std::vector<double>& a);

int hamming_distance(const facility_vector& a, const facility_vector& b);
// Returns ||v||_2
double magnitude(const std::vector<double>& v);

std::string primitive_list_to_string(const std::vector<int>& container);
std::string primitive_list_to_string(const std::vector<double>& container);

// Generic functions
template<typename T>
bool contains(const std::vector<T>& container, const T& x);

template<typename T1, typename T2>
std::vector<T1> projection_1_2(const std::vector<std::pair<T1, T2>>& pairs);

template<typename T>
void bisect_insert(std::vector<T>& container, const T& x, const std::function<bool(const T&, const T&)>& comes_before);

#include "data_helper_impl.h"
#endif