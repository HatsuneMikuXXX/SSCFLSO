#ifndef STINGY_H
#define STINGY_H
#include "../algorithm.h"
/// After preprocessing, remove iteratively that facility that removes the highest cost while maintaining facilities.
/// Considerate: Empty facilities 
facility_vector stingy(const SSCFLSO& instance);
#endif