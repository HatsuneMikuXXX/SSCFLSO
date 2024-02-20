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

template<typename T>
void bisect_insert(std::vector<T>& container, const T& x, std::function<bool(T, T)> comes_before) {
	// Initial case
	if (container.size() == 0) {
		container.push_back(x);
		return;
	}
	// General case - We need at least n comparisons where |container| < 2^n
	int index = (container.size() - 1)/ 2; // Note: CPP always rounds down
	// First n-1 comparisons for finding the last index
	int step = container.size() / 2;
	for (int loops = 1; 2 * loops <= container.size(); loops *= 2) {
		step = (step + 1) / 2;
		if (comes_before(x, container[index])) {
			index -= (index == 0) ? 0 : step;
		}
		else {
			index += (index == container.size() - 1) ? 0 : step;
		}
	}
	// Last comparison for insertion
	if (comes_before(x, container[index])) {
		container.insert(container.begin() + index, x);
	}
	else {
		container.insert(container.begin() + (index + 1), x);
	}
	// TODO REMOVE
	for (int i = 0; i < container.size() - 1; i++) {
		assert(comes_before(container[i], container[i + 1]));
	}
}