#include "../common.h"

template<typename T>
bool contains(const std::vector<T>& container, const T& x){
	return (find(container.begin(), container.end(), x) != container.end());
}

template<typename T1, typename T2>
std::vector<T1> get_firsts(const std::vector<std::pair<T1, T2>>& pairs){
	std::vector<T1> filtered;
	auto lambda = [](std::pair<T1, T2> pair){ return pair.first; };
	transform(pairs.begin(), pairs.end(), back_inserter(filtered), lambda);
	return filtered;
}