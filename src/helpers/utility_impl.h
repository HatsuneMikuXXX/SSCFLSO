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
void insert_sorted_container(std::vector<T>& sorted_container, const T& to_insert, const std::function<bool(const T&, const T&)> LEQ) {
	sorted_container.insert(std::upper_bound(sorted_container.begin(), sorted_container.end(), to_insert, LEQ), to_insert);
}