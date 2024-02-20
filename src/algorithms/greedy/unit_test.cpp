#include "unit_test.h"
namespace GreedyUnitTest{
	bool run_tests(){
		try{
			std::cout << "Running Greedy Test 1..." << std::endl;
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
		if (sum(greedy_with_popularity(y)) > 0) {
			throw std::runtime_error("Solution space is empty but Greedy (with popularity) returned a feasible solution");
		}
		if (sum(greedy_without_popularity(y)) > 0) {
			throw std::runtime_error("Solution space is empty but Greedy (without popularity) returned a feasible solution");
		}
		y = Generator::load_instance("instances/unit_tests/unit_test_small.plc");
		facility_vector res = greedy_with_popularity(y);
		if (!(sum(res) == 2 && res[0] == 1 && res[1] == 1)) {
			throw std::runtime_error("The returned solution should be of size 2 and contain the facilities 1 and 2.");
		}
		facility_vector res = greedy_without_popularity(y);
		if (!(sum(res) == 2 && res[2] == 0)) {
			throw std::runtime_error("The returned solution should be of size 2 and must not contain facility 2.");
		}
	}
}
