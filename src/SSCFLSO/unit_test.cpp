#include "unit_test.h"
using namespace std;

#define EPSILON __DBL_MIN__
const string ABSPATH = "C:/Users/Giang/VSCode/projects/SSCFLSO/";

bool areSame(float a, float b){
	return fabs(a - b) < 0.000001;
	
}

bool run_all(){
	test1();
	test2(); // Requires test 1 to be successful
	test3(); // Requires test 2 to be successful
	return true;
}

void test1(){
	// Test out Generator methods.
	Generator x = Generator(3, 5);
	// Properly initialized?
	try{
		float access = x.get_instance().demands[4];
		access = x.get_instance().capacities[2];
		access = x.get_instance().open_costs[2];
		access = x.get_instance().dist_costs[2][4];
		access = x.get_instance().preferences[4][2];
	}
	catch(const exception& e){
		cout << "Access failed.";
	}
	// Assign values correctly?
	x.set_demand(3, 8.31);
	x.set_capacity(1, 35.917);
	x.set_open_cost(2, 900);
	x.set_dist_cost(0, 4, 25.34);
	assert(areSame(x.get_instance().demands[3], 8.31));
	assert(areSame(x.get_instance().capacities[1], 35.917));
	assert(areSame(x.get_instance().open_costs[2], 900));
	assert(areSame(x.get_instance().dist_costs[0][4], 25.34));
	
	x.set_dist_cost(0, 2, 6);
	x.set_dist_cost(1, 2, 7);
	x.set_dist_cost(2, 2, 5);

	x.set_dist_cost(0, 3, 10);
	x.set_dist_cost(1, 3, 0);
	x.set_dist_cost(2, 3, 1);
	x.set_preferences(0);
	vector<int> y1{2, 0, 1};
	vector<int> y2{1, 2, 0};
	assert(x.get_instance().preferences[2] == y1);
	assert(x.get_instance().preferences[3] == y2);
	cout << "Passed Test 1.\n";
}

void test2(){
	// Test saving and loading.
	Generator x = Generator(3, 3);
	vector<float> demands{1.25, 1.5, 2.25};
	vector<float> capacities{5, 2.75, 2.25};
	vector<float> open_costs{2.7, 2.69, 2.71};
	vector<vector<float>> dist_costs{{0.1, 1, 3.1}, {2, 0.5, 1}, {2.5, 1.41, 0.12}};

	for(int i = 0; i < 3; i++){
		x.set_demand(i, demands[i]);
		x.set_capacity(i, capacities[i]);
		x.set_open_cost(i, open_costs[i]);
		for(int j = 0; j < 3; j++){
			x.set_dist_cost(i, j, dist_costs[i][j]);
		}
	}
	x.set_preferences(0);
	SSCFLSO y = x.get_instance();
	x.save_instance(&y, ABSPATH + "instances/custom/unit_test0.plc", true);
	// Load instance
	y = x.load_instance(ABSPATH + "instances/custom/unit_test0.plc");
	assert(y.demands == demands);
	assert(y.capacities == capacities);
	assert(y.open_costs == open_costs);
	assert(y.dist_costs == dist_costs);

	// Overwrite safe
	x.set_demand(0, 5000);
	y = x.get_instance();
	x.save_instance(&y, ABSPATH + "instances/custom/unit_test0.plc", false);
	y = x.load_instance(ABSPATH + "instances/custom/unit_test0.plc");
	assert(areSame(y.demands[0], 1.25));
	cout << "Passed Test 2.\n";
}

void test3(){
	SSCFLSO y = Generator::load_instance(ABSPATH + "instances/custom/unit_test0.plc");
	Validator FLV = Validator(&y);

	FLV.set_solution({0});
	assert((FLV.get_assignment() == vector<int>{0, 0, 0}));
	assert(areSame(FLV.value(), 6.9));
	assert(FLV.feasible());

	FLV.set_solution({0, 1});
	assert((FLV.get_assignment() == vector<int>{0, 1, 1}));
	assert(areSame(FLV.value(), 6.99));
	assert(!FLV.feasible());

	FLV.set_solution({0});
	assert((FLV.get_assignment() == vector<int>{0, 0, 0}));
	assert(areSame(FLV.value(), 6.9));
	assert(FLV.feasible());

	FLV.set_solution({0, 1, 2});
	assert((FLV.get_assignment() == vector<int>{0, 1, 2}));
	assert(areSame(FLV.value(), 8.82));
	assert(FLV.feasible());

	cout << "Passed Test 3.\n";
}