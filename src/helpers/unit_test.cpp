#include "unit_test.h"

bool HelpersUnitTest::run_tests() {
	try {
		std::cout << "Running Helper Test 1..." << std::endl;
		test1();
		std::cout << "OK\nRunning Helper Test 2..." << std::endl;
		test2();
		std::cout << "OK" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void HelpersUnitTest::test1() {
	// Inverse
	std::vector<int> to_inverse = {3, 0, 5, 4, 1, 6, 2, 7};
	std::vector<int> result = {1,4,6,0,3,2,5,7};
	assert(inverse(to_inverse) == result);
}

void HelpersUnitTest::test2() {
	// Bisect Insert
	std::vector<int> to_sort = { 2, 3, 5, 7, 0, 1, 8, 9, 11, 13, 17, 4, 6, 10, 14, 15, 16, 12, 18 };
	std::vector<int> result = {};
	const std::function<bool(const int&, const int&)> is_less = [](int a, int b) {return a < b; };
	for (int i = 0; i < to_sort.size(); i++) {
		bisect_insert(result, to_sort[i], is_less);
	}
	for (int i = 0; i < to_sort.size(); i++) {
		assert(result[i] == i);
	}
}