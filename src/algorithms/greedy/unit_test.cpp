#include "unit_test.h"
namespace GreedyUnitTest{
	bool run_tests(){
		try{
			test1();
		}
		catch(const std::exception& e){
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}
	
	void test1(){
		SSCFLSO y = Generator::load_instance("src/algorithms/greedy/unit_test_stingy0.plc");
		if(!greedy(y).empty()){
			throw std::runtime_error("Solution space is empty but Greedy returned a feasible solution");
		}
		y = Generator::load_instance("src/algorithms/stingy/unit_test_stingy1.plc");
		facility_vector res = greedy(y);
		if(!(res != std::vector<int>{1,2,3})){
			throw std::runtime_error("The returned solution is different from what is expected.");
		}
	}
}
