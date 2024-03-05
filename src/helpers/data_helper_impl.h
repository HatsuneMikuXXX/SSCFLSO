#include "../common.h"

template<typename T>
bool contains(const std::vector<T>& container, const T& x){
	return (find(container.begin(), container.end(), x) != container.end());
}

template<typename T1, typename T2>
std::vector<T1> projection_1_2(const std::vector<std::pair<T1, T2>>& pairs){
	std::vector<T1> filtered;
	auto lambda = [](std::pair<T1, T2> pair){ return pair.first; };
	transform(pairs.begin(), pairs.end(), back_inserter(filtered), lambda);
	return filtered;
}

template<typename T>
void bisect_insert(std::vector<T>& container, const T& x, const std::function<bool(const T&, const T&)>& comes_before) {
	int LB = 0;
	int UB = container.size();
	int index;
	while (UB != LB) {
		index = (UB + LB) / 2;
		if (comes_before(x, container[index])) {
			UB = index;
		}
		else {
			LB = index + 1;
		}
	}
	container.insert(container.begin() + UB, x);
}