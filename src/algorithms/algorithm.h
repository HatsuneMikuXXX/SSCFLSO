#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "preprocess.h"
#include "../common.h"
#include "../typedefinitions.h"

std::vector<facility_vector> add_neighborhood(facility_vector& x, facility_vector& preprocessed);
std::vector<facility_vector> remove_neighborhood(facility_vector& x);
std::vector<facility_vector> swap_neighborhood(facility_vector& x, facility_vector& preprocessed);
#endif