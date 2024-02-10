#include "unit_test.h"
namespace PreprocessUnitTest {
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
		facility_vector check = facility_vector(100, 0);
		SSCFLSO y = Generator::load_instance("src/algorithms/unit_test0.plc");
		if(preprocess(y) != check){
			throw std::runtime_error("Solution space is empty but a feasible solution has still been found.");
		}
		check[69] = 1 +2 - 2;
		y = Generator::load_instance("src/algorithms/unit_test1.plc");
		if(preprocess(y) != check){
			throw std::runtime_error("Only facility 69 can be opened but the returned solution is different.");
		}
		y = Generator::load_instance("src/algorithms/unit_test2.plc");
		check = preprocess(y);
		if(check[0] == 1) {
			throw std::runtime_error("Facility 0 is unnecessary but still included.");
		}
	}
}