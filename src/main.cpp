#include "algorithms/stingy/unit_test.h"
#include "algorithms/greedy/unit_test.h"
#include "algorithms/local_search/unit_test.h"
#include "SSCFLSO/unit_test.h"
#include "helpers/unit_test.h"
int main()
{
    // Test your implemented stuff to check correctess.
    bool x = LocalSearchUnitTest::run_tests();
    if(x){
        std::cout << "Nice work." << std::endl;
    }
    return 0;
}