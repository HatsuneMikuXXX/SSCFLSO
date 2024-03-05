#include "libraries.h"

typedef std::vector<int> solution;
typedef std::function<solution(SSCFLSO&)> algorithm;

int main()
{
    bool success = RoundingUnitTest::run_tests();
    return success;
}
