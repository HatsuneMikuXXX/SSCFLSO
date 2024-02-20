#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "preprocess.h"
#include "../common.h"
#include "../typedefinitions.h"

std::vector<facility_vector> add_neighborhood(const facility_vector& x, const facility_vector& preprocessed);
std::vector<facility_vector> remove_neighborhood(const facility_vector& x);
std::vector<facility_vector> swap_neighborhood(const facility_vector& x, const facility_vector& preprocessed);
#endif