#ifndef HELPER_H
#define HELPER_H
#include <vector>
#include <functional> // For filter
#include <math.h> // For fabs
#include "../constants.h"
#include "data_helper_impl.h"
/// Return "`x` is element of `container`"
template<typename T>
bool contains(const std::vector<T>& container, const T& x);
/// Append values to the container in the order of the permutation (keys), i.e. add values[keys[0]], values[keys[1]],...
template<typename T>
void permutated_fill(const std::vector<T>& keys, const std::vector<T>& values, std::vector<T>& container);
/// @brief Get a vector.
/// @param n Size of the vector
/// @return The vector {0,...,n-1} 
std::vector<int> range(int n);
/// @brief Keep those elements that fulfill the predicate
/// @tparam T 
/// @param container 
/// @param predicate 
template<typename T>
void filter(std::vector<T>& container, const std::function<bool(T)>& predicate);
/// Project vector of pairs onto their first elements
template<typename T1, typename T2>
std::vector<T1> get_firsts(const std::vector<std::pair<T1, T2>>& pairs);
/// @brief Checks if two floats are the same up to a certain error.
/// @param a 
/// @param b 
/// @return 
bool areSame(float a, float b);
/// @brief Returns the string representation of a vector, that is [v1,v2,...]
/// @param  
/// @return 
std::string solution_to_string(const std::vector<int>&);
std::string solution_to_string(const std::vector<float>&);
#endif