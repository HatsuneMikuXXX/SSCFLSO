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

extern const std::function<bool(const std::pair<int, double>&, const std::pair<int, double>&)> sort_by_first;
extern const std::function<bool(const std::pair<int, double>&, const std::pair<int, double>&)> sort_by_second;

/// Inverse index set, example: [3,0,1,4,5,2,6] becomes [1,2,5,0,3,4,6]
std::vector<int> inverse(const std::vector<int>& index_set);

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

bool is_empty(const facility_vector& a);

// Generic functions
template<typename T>
bool contains(const std::vector<T>& container, const T& x);

// Project a list of pairs on to their first elements
template<typename T1, typename T2>
std::vector<T1> projection_1_2(const std::vector<std::pair<T1, T2>>& pairs);

template<typename T>
void insert_sorted_container(std::vector<T>& sorted_container, const T& to_insert, const std::function<bool(const T&, const T&)> LEQ);

#include "utility_impl.h"
#endif