#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H
#include "../algorithm.h"
/// Strategy: Take first feasible neighbor found. Order of neighborhood relations to check: add, remove, swap.
facility_vector local_search(const SSCFLSO& instance);
/// @brief Add one single facility.
/// @param FLV 
/// @param open_facilities 
/// @param closed_facilities 
/// @return 
bool add(Validator& FLV, facility_vector& open_facilities, facility_vector& closed_facilities, double& current_value);
/// @brief Remove one single facility.
/// @param FLV 
/// @param open_facilities 
/// @param closed_facilities 
/// @return 
bool remove(Validator& FLV, facility_vector& open_facilities, facility_vector& closed_facilities, double& current_value);
/// Add one facility and remove a different one.
bool swap(Validator& FLV, facility_vector& open_facilities, facility_vector& closed_facilities, double& current_value);
#endif