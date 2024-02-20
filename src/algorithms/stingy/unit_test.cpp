#include "unit_test.h"
namespace StingyUnitTest{
	bool run_tests(){
		try{
			std::cout << "Running Stingy Test 1..." << std::endl;
			test1();
			std::cout << "OK" << std::endl;
		}
		catch(const std::exception& e){
			std::cerr << e.what() << std::endl;
			return false;
		}
		return true;
	}
	
	void test1(){
		SSCFLSO y = Generator::load_instance("instances/unit_tests/unit_test_infeasible0.plc");
		if(sum(stingy(y)) > 0){
			throw std::runtime_error("Solution space is empty but Stingy returned a feasible solution");
		}
		y = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
		facility_vector res = stingy(y);
		if(!(sum(res) == 3 && res[2] == 1 && res[3] == 1)) {
			throw std::runtime_error("The returned solution should be of size 3 and contain the facilities 2 and 3.");
		}
	}
}
