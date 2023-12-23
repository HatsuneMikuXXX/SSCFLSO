#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

template<typename T>
bool contains(const std::vector<T>& container, const T& x){
	return (find(container.begin(), container.end(), x) != container.end());
}

template<typename T>
void permutated_fill(const std::vector<T>& keys, const std::vector<T>& values, std::vector<T>& container){
	int original_size = container.size();
	container.resize(original_size + keys.size());
	for(int i = 0; i < keys.size(); i++){
		container[original_size + keys[i]] = values[i];
	}
}

template<typename T>
void filter(std::vector<T>& container, const std::function<bool(T)>& predicate){
	container.erase(std::remove_if(container.begin(), container.end(), std::not1(predicate)), container.end());
}

template<typename T1, typename T2>
std::vector<T1> get_firsts(const std::vector<std::pair<T1, T2>>& pairs){
	std::vector<T1> filtered;
	auto lambda = [](std::pair<T1, T2> pair){ return pair.first; };
	transform(pairs.begin(), pairs.end(), back_inserter(filtered), lambda);
	return filtered;
}