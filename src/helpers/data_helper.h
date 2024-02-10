#ifndef HELPER_H
#define HELPER_H
#include "../common.h"
#include "../constants.h"
#include "../typedefinitions.h"
#include "data_helper_impl.h"
/// Return "`x` is element of `container`"
template<typename T>
bool contains(const std::vector<T>& container, const T& x);
/// Inverse index set, example: [3,0,1,4,5,2,6] becomes []
std::vector<int> inverse(const std::vector<int> set);
/// @brief Get a vector.
/// @param n Size of the vector
/// @return The vector {0,...,n-1} 
std::vector<int> range(int n);
/// @brief Set those 1-entries to 0 if they do not fulfill the predicate
/// @param facilities 
/// @param predicate 
void filter(facility_vector& facilities, facility_predicate& predicate);
/// Project vector of pairs onto their first elements
template<typename T1, typename T2>
std::vector<T1> get_firsts(const std::vector<std::pair<T1, T2>>& pairs);
/// @brief Checks if two doubles (or certain containers that have doubles) are the same up to a certain error.
/// @param a 
/// @param b 
/// @return 
bool areSame(double a, double b);
bool areSame(std::vector<double>& a, std::vector<double>& b);
bool areSame(std::vector<std::vector<double>>& a, std::vector<std::vector<double>>& b);

int sum(std::vector<int>& a);
double sum(std::vector<double>& a);

int hamming_distance(facility_vector& a, facility_vector& b);
/// @brief Returns the string representation of a vector, that is [v1,v2,...]
/// @param  
/// @return 
std::string solution_to_string(const std::vector<int>&);
std::string solution_to_string(const std::vector<double>&);
#endif