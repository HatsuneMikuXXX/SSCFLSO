#include "headers.h"
#include "benchmarking/benchmark.h"

int main(int argc, char* argv[])
{
    std::cout << "Starting SSCFLSO Application" << std::endl;
    srand(17);
    if(argc != 4) {
        std::cout << "Wrong number of algorithms" << std::endl;
        return 0;
    }

    int algo_code = -1;
    bool grb = true;
    Algorithm* algo = NULL;
    std::vector<Algorithm*> composition(0);
    try{
        algo_code = std::stoi(argv[3]);
    }
    catch(...){
        std::cout << "Invalid algo code. Accepting only single digit";
        return 1;
    }
    switch(algo_code) {
        case 0:
            algo = new Gurobi();
            grb = false;
            break;
        case 1:
            algo = new LocalSearch(LocalSearch::PREPROCESS, LocalSearch::BEST);
            break;
        case 2:
            algo = new LocalSearch(LocalSearch::PREPROCESS, LocalSearch::FIRST);
            break;
        case 3:
            composition.push_back(new Rounding());
            composition.push_back(new LocalSearch(LocalSearch::GIVEN, LocalSearch::BEST));
            algo = new Composite(composition);
            break;
        case 4:
            composition.push_back(new Rounding());
            composition.push_back(new LocalSearch(LocalSearch::GIVEN, LocalSearch::FIRST));
            algo = new Composite(composition);
            break;
        case 5:
            algo = new EagerSearch();
            grb = false;
            break;
        case 6:
            algo = new SemiLagrangianRelaxation(false);
            grb = false;
            break;
        case 7:
            algo = new SemiLagrangianRelaxation(true);
            grb = false;
            break;
        case 8:
            algo = new LagrangianRelaxation();
            break;
        case 9:
            algo = new GRASP(0.5);
            grb = false;
            break;
        default:
            return -1;
    }
    {
         try {
            const SSCFLSO instance = Generator::load_instance(argv[1], true);
            std::string filename = argv[1];
            size_t pos = 0;
            while ((pos = filename.find("/")) != std::string::npos) {
                filename.erase(0, pos + 1);
            }
            run(instance, filename, argv[2], 0, algo, grb); // Time limit does not matter
        }
        catch (std::runtime_error e) {
            std::cout << "Instance has wrong format or couldn't be found." << std::endl;
        }
    }
    return 0;
}
