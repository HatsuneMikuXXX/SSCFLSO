#ifndef STINGY_H
#define STINGY_H
#include "../algorithmClass.h"
/// After preprocessing, remove iteratively that facility that removes the highest cost while maintaining facilities.

class Stingy : public Algorithm {
	public:
		static void solve(const SSCFLSO& instance, facility_vector& current_best, const std::chrono::milliseconds& time_limit);
		static std::string meta_information();
};
#endif