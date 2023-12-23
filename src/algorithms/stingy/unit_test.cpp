#include "unit_test.h"
namespace StingyUnitTest{
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
		SSCFLSO y = Generator::load_instance("src/algorithms/stingy/unit_test_stingy0.plc");
		if(!stingy(y).empty()){
			throw std::runtime_error("Solution space is empty but Stingy returned a feasible solution");
		}
		y = Generator::load_instance("src/algorithms/stingy/unit_test_stingy1.plc");
		solution res = stingy(y);
		if(!(res.size() == 3 && contains(res, 2) && contains(res, 3))){
			throw std::runtime_error("The returned solution should be of size 3 and contain the facilities 2 and 3.");
		}
	}
}
