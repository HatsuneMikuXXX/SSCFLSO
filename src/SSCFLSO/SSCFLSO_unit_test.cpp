#include "SSCFLSO_unit_test.h"

bool SSCFLSOUnitTest::run_tests(){
	try{
		std::cout << "Running SSCFLSO Test 1: Create and update instance correctly." << std::endl;
		test1();
		std::cout << "OK\nRunning SSCFLSO Test 2: Save and load." << std::endl;
		test2();
		std::cout << "OK\nRunning SSCFLSO Test 3: Assignment, Objective value, Feasibility and number of capacity exceeding facilities." << std::endl;
		test3();
		std::cout << "OK\nRunning SSCFLSO Test 4: Remove non-supplying facilities." << std::endl;
		test4();
		std::cout << "OK" << std::endl;
	}
	catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void SSCFLSOUnitTest::test1(){
	Generator GeneratorObject = Generator(3, 5);
	try{
		double access = GeneratorObject.get_instance().demands[4];
		access = GeneratorObject.get_instance().capacities[2];
		access = GeneratorObject.get_instance().facility_costs[2];
		access = GeneratorObject.get_instance().distribution_costs[2][4];
		access = GeneratorObject.get_instance().preferences[4][2];
	}
	catch(const std::exception& e){
		std::cerr << e.what();
		throw std::runtime_error("Instance initialization failed. Accessing attributes lead to segmentation fault.");
	}
	GeneratorObject.set_demand(3, 8.31);
	GeneratorObject.set_capacity(1, 35.917);
	GeneratorObject.set_facility_cost(2, 900);
	GeneratorObject.set_distribution_cost(0, 4, 25.34);
	bool a1 = are_same(GeneratorObject.get_instance().demands[3], 8.31);
	bool a2 = are_same(GeneratorObject.get_instance().capacities[1], 35.917);
	bool a3 = are_same(GeneratorObject.get_instance().facility_costs[2], 900);
	bool a4 = are_same(GeneratorObject.get_instance().distribution_costs[0][4], 25.34);
	if(!(a1 && a2 && a3 && a4)){
		throw std::runtime_error("Setting values failed.");
	}
	GeneratorObject.set_distribution_cost(0, 2, 6);
	GeneratorObject.set_distribution_cost(1, 2, 7);
	GeneratorObject.set_distribution_cost(2, 2, 5);
	GeneratorObject.set_distribution_cost(0, 3, 10);
	GeneratorObject.set_distribution_cost(1, 3, 0);
	GeneratorObject.set_distribution_cost(2, 3, 1);
	GeneratorObject.set_preferences(Generator::Category::closest_assignment);
	const std::vector<int> y1{2, 0, 1};
	const std::vector<int> y2{1, 2, 0};
	const bool b1 = GeneratorObject.get_instance().preferences[2] == y1;
	const bool b2 = GeneratorObject.get_instance().preferences[3] == y2;
	if(!(b1 && b2)){
		throw std::runtime_error("Setting preferences using closest assignment failed.");
	}
}

void SSCFLSOUnitTest::test2(){
	Generator GeneratorObject = Generator(3, 3);
	demand_vector demands{1.25, 1.5, 2.25};
	capacity_vector capacities{5, 2.75, 2.25};
	facility_cost_vector facility_costs{2.7, 2.69, 2.71};
	distribution_cost_matrix distribution_costs{{0.1, 1, 3.1}, {2, 0.5, 1}, {2.5, 1.41, 0.12}};
	for(int i = 0; i < 3; i++){
		GeneratorObject.set_demand(i, demands[i]);
		GeneratorObject.set_capacity(i, capacities[i]);
		GeneratorObject.set_facility_cost(i, facility_costs[i]);
		for(int j = 0; j < 3; j++){
			GeneratorObject.set_distribution_cost(i, j, distribution_costs[i][j]);
		}
	}
	GeneratorObject.set_preferences(Generator::Category::closest_assignment);
	SSCFLSO SSCFLSOObject = GeneratorObject.get_instance();
	GeneratorObject.save_instance(SSCFLSOObject, "instances/unit_tests/unit_test_SSCFLSO.plc", true);
	SSCFLSOObject = GeneratorObject.load_instance("instances/unit_tests/unit_test_SSCFLSO.plc", true);
	const bool c1 = are_same(SSCFLSOObject.demands, demands);
	const bool c2 = are_same(SSCFLSOObject.capacities, capacities); 
	const bool c3 = are_same(SSCFLSOObject.facility_costs, facility_costs); 
	const bool c4 = are_same(SSCFLSOObject.distribution_costs, distribution_costs);
	if(!(c1 && c2 && c3 && c4)){
		throw std::runtime_error("Saving failed.");
	}
	GeneratorObject.set_demand(0, 5000);
	SSCFLSOObject = GeneratorObject.get_instance();
	GeneratorObject.save_instance(SSCFLSOObject, "instances/unit_tests/unit_test_SSCFLSO.plc", false);
	SSCFLSOObject = GeneratorObject.load_instance("instances/unit_tests/unit_test_SSCFLSO.plc", true);
	const bool d1 = are_same(SSCFLSOObject.demands[0], 1.25);
	if(!d1){
		throw std::runtime_error("File was overwritten but shouldn't!");
	}
}

void SSCFLSOUnitTest::test3(){
	SSCFLSO SSCFLSOObject = Generator::load_instance("instances/unit_tests/unit_test_SSCFLSO.plc", true);
	Validator FLV = Validator(SSCFLSOObject);
	FLV.set_solution({1, 0, 0});
	const bool assign1 = FLV.get_assignment() == client_facility_assignment({0, 0, 0});
	const bool value1 = are_same(FLV.value(), 6.9);
	const bool feasible1 = FLV.feasible();
	FLV.set_solution({ 1, 1, 0 });
	const bool assign2 = FLV.get_assignment() == client_facility_assignment({ 0, 1, 1 });
	const bool value2 = are_same(FLV.value(), 6.99);
	const bool feasible2 = FLV.feasible();
	const bool violation2 = FLV.exceeds_capacity() == facility_vector({ 0, 1, 0 });
	FLV.set_solution({ 1, 0, 0 });
	const bool assign3 = FLV.get_assignment() == client_facility_assignment({ 0, 0, 0 });
	const bool value3 = are_same(FLV.value(), 6.9);
	const bool feasible3 = FLV.feasible();
	FLV.set_solution({ 1, 1, 1 });
	const bool assign4 = FLV.get_assignment() == client_facility_assignment({ 0, 1, 2 });
	const bool value4 = are_same(FLV.value(), 8.82);
	const bool feasible4 = FLV.feasible();
	if (!(assign1 && assign2 && assign3 && assign4)) {
		throw std::runtime_error("Assignment wrong!");
	}
	if (!(value1 && value2 && value3 && value4)) {
		throw std::runtime_error("Value computed wrongly!");
	}
	if (!(feasible1 && !feasible2 && feasible3 && feasible4)) {
		throw std::runtime_error("Feasibility computed wrongly!");
	}
	if (!violation2) {
		throw std::runtime_error("Facilities that violate capacity limits computed wrongly!");
	}
}

void SSCFLSOUnitTest::test4() {
	SSCFLSO SSCFLSOObject = Generator::load_instance("instances/unit_tests/unit_test_dropEmptyTest.plc", true);
	Validator FLV = Validator(SSCFLSOObject);
	FLV.set_solution(facility_vector({1, 1}));
	FLV.drop_empty_facilities();
	if (FLV.get_solution() != facility_vector({ 1, 0 })) {
		throw std::runtime_error("Dropping empty facilities failed!");
	}
}
