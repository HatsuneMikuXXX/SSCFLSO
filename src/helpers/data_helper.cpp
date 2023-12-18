#include <vector>
using namespace std;

template<typename T>
bool contains(vector<T>* container, T x){
	return (find(container->begin(), container->end(), x) != container->end());
};

template<typename T1, typename T2>
vector<T1> get_firsts(vector<pair<T1, T2>>* pairs){
	vector<T1> filtered;
	auto lambda = [](pair<T1, T2> pair){ return pair.first; };
	transform(pairs->begin(), pairs->end(), back_inserter(filtered), lambda);
	return filtered;
};