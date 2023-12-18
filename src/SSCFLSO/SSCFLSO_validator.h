#include <cmath>
#include "SSCFLSO_generator.h"
using namespace std;

class Validator {
	public:
		Validator(SSCFLSO*);
		void set_solution(vector<int>);
		vector<int> get_assignment();
		float value();
		bool feasible();
	private:
		SSCFLSO* ptr_instance;
		vector<vector<int>> rankings; // Let f = preferences[i][k] for client i and some index k. Then rankings[i][f] = k.
		vector<int> open_facilities;
		float solution_value;
		vector<int> assignment; //Index = Facility, Value = Client
};