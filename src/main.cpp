#include "algorithms/stingy/unit_test.h"
#include "algorithms/greedy/unit_test.h"
#include "algorithms/local_search/unit_test.h"
#include "SSCFLSO/unit_test.h"
#include "helpers/unit_test.h"
#include "helpers/benchmark.h"

typedef std::vector<int> solution;
typedef std::function<solution(SSCFLSO&)> algorithm;

int main()
{
    // Test your implemented stuff to check correctess.
    algorithm ls = [](SSCFLSO& instance){ return local_search(instance); };
    algorithm gr = [](SSCFLSO& instance){ return greedy(instance); };
    algorithm st = [](SSCFLSO& instance){ return stingy(instance); };
    std::vector<std::pair<std::string, algorithm>> algorithms{
        std::pair<std::string, algorithm>("L", ls),
        std::pair<std::string, algorithm>("G", gr),
        std::pair<std::string, algorithm>("S", st)
    };
    std::vector<std::string> instances{};
    for(int i = 1; i < 21; i++){
        std::string s = "instances/i300/i300_";
        s += std::to_string(i) + ".plc";
        instances.push_back(s);
    }
    run(algorithms, instances, "results/output0.txt", 0, false, 4);
    return 0;
}