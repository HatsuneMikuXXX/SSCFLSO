#include "libraries.h"

typedef std::vector<int> solution;
typedef std::function<solution(SSCFLSO&)> algorithm;

int main()
{
    bool success = TESTER::run_all_tests();
    return success;
}