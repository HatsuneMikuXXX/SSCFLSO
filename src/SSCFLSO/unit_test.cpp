#include "unit_test.h"
namespace SSCFLSOUnitTest{
	bool run_tests(){
		try{
			test1();
			test2();
			test3();
		}
		catch(const std::exception& e){
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}

	void test1(){
		Generator x = Generator(3, 5);
		try{
			double access = x.get_instance().demands[4];
			access = x.get_instance().capacities[2];
			access = x.get_instance().facility_costs[2];
			access = x.get_instance().distribution_costs[2][4];
			access = x.get_instance().preferences[4][2];
		}
		catch(const std::exception& _){
			throw std::runtime_error("Instance initialization failed. Accessing attributes lead to segmentation fault.");
		}
		x.set_demand(3, 8.31);
		x.set_capacity(1, 35.917);
		x.set_facility_cost(2, 900);
		x.set_distribution_cost(0, 4, 25.34);
		bool a1 = areSame(x.get_instance().demands[3], 8.31);
		bool a2 = areSame(x.get_instance().capacities[1], 35.917);
		bool a3 = areSame(x.get_instance().facility_costs[2], 900);
		bool a4 = areSame(x.get_instance().distribution_costs[0][4], 25.34);
		if(!(a1 && a2 && a3 && a4)){
			throw std::runtime_error("Setting values failed.");
		}
		x.set_distribution_cost(0, 2, 6);
		x.set_distribution_cost(1, 2, 7);
		x.set_distribution_cost(2, 2, 5);
		x.set_distribution_cost(0, 3, 10);
		x.set_distribution_cost(1, 3, 0);
		x.set_distribution_cost(2, 3, 1);
		x.set_preferences(Generator::Category::cooperative);
		std::vector<int> y1{2, 0, 1};
		std::vector<int> y2{1, 2, 0};
		bool b1 = x.get_instance().preferences[2] == y1;
		bool b2 = x.get_instance().preferences[3] == y2;
		if(!(b1 && b2)){
			throw std::runtime_error("Setting preferences by distance failed.");
		}
	}

	void test2(){
		Generator x = Generator(3, 3);
		demand_vector demands{1.25, 1.5, 2.25};
		capacity_vector capacities{5, 2.75, 2.25};
		facility_cost_vector facility_costs{2.7, 2.69, 2.71};
		distribution_cost_matrix distribution_costs{{0.1, 1, 3.1}, {2, 0.5, 1}, {2.5, 1.41, 0.12}};
		for(int i = 0; i < 3; i++){
			x.set_demand(i, demands[i]);
			x.set_capacity(i, capacities[i]);
			x.set_facility_cost(i, facility_costs[i]);
			for(int j = 0; j < 3; j++){
				x.set_distribution_cost(i, j, distribution_costs[i][j]);
			}
		}
		x.set_preferences(Generator::Category::cooperative);
		SSCFLSO y = x.get_instance();
		x.save_instance(y, "src/SSCFLSO/unit_test0.plc", true);
		y = x.load_instance("src/SSCFLSO/unit_test0.plc");
		bool c1 = areSame(y.demands, demands);
		bool c2 = areSame(y.capacities, capacities); 
		bool c3 = areSame(y.facility_costs, facility_costs); 
		bool c4 = areSame(y.distribution_costs, distribution_costs);
		if(!(c1 && c2 && c3 && c4)){
			throw std::runtime_error("Saving failed.");
		}
		x.set_demand(0, 5000);
		y = x.get_instance();
		x.save_instance(y, "src/SSCFLSO/unit_test0.plc", false);
		y = x.load_instance("src/SSCFLSO/unit_test0.plc");
		bool d1 = areSame(y.demands[0], 1.25);
		if(!d1){
			throw std::runtime_error("File was overwritten but shouldn't!");
		}
	}

	void test3(){
		SSCFLSO y = Generator::load_instance("src/SSCFLSO/unit_test0.plc");
		Validator FLV = Validator(y);
		FLV.set_solution({1, 0, 0});
		bool assign1 = FLV.get_assignment() == client_facility_assignment{0, 0, 0};
		bool value1 = areSame(FLV.value(), 6.9);
		bool feasible1 = FLV.feasible();
		FLV.set_solution({1, 1, 0});
		bool assign2 = FLV.get_assignment() == client_facility_assignment{0, 1, 1};
		bool value2 = areSame(FLV.value(), 6.99);
		bool feasible2 = FLV.feasible();
		bool violation2 = FLV.exceeds_capacity() == client_facility_assignment{0, 1, 0};
		FLV.set_solution({1, 0, 0});
		bool assign3 = FLV.get_assignment() == client_facility_assignment{0, 0, 0};
		bool value3 = areSame(FLV.value(), 6.9);
		bool feasible3 = FLV.feasible();
		FLV.set_solution({1, 1, 1});
		bool assign4 = FLV.get_assignment() == client_facility_assignment{0, 1, 2};
		bool value4 = areSame(FLV.value(), 8.82);
		bool feasible4 = FLV.feasible();
		if(!(assign1 && assign2 && assign3 && assign4)){
			throw std::runtime_error("Assignment wrong!");
		}
		if(!(value1 && value2 && value3 && value4)){
			throw std::runtime_error("Value computed wrongly!");
		}
		if(!(feasible1 && !feasible2 && feasible3 && feasible4)){
			throw std::runtime_error("Feasibility computed wrongly!");
		}
		if(!violation2){
			throw std::runtime_error("Facilities that violate capacity limits computed wrongly!");
		}
	}
}