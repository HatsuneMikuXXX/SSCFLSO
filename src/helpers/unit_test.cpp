#include "unit_test.h"
namespace HelpersUnitTest{
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
		SSCFLSO y = Generator::load_instance("src/helpers/unit_test0.plc");
		if(preprocess(y) != std::vector<int>{}){
			throw std::runtime_error("Solution space is empty but a feasible solution has been found.");
		}
		y = Generator::load_instance("src/helpers/unit_test1.plc");
		if(preprocess(y) != std::vector<int>{69}){
			throw std::runtime_error("Only facility 69 can be opened but the returned solution is different.");
		}
	}
}