#include "libraries.h"

int main()
{
    /*
    std::string file_prefix = "instances/i300/i300_";
    srand(7);
    for (int i = 1; i < 21; i++) {
        std::string file_i = file_prefix + std::to_string(i) + ".plc";
        SSCFLSO x = Generator::load_instance(file_i, false, Generator::linear_bias);
        Generator::save_instance(x, "instances/i300_with_perturbed_preferences/i300_seed(7)_" + std::to_string(i) + "_perturbed_preferences.plc", true);
    }*/
    LocalSearch gp = LocalSearch();
    std::string file_prefix = "instances/i300_with_cooperative_preferences/i300_";
    for (int i = 1; i < 21; i++) {
        std::cout << "Iteration " << i << std::endl;
        std::string file_i = file_prefix + std::to_string(i) + "_cooperative_preferences.plc";
        std::string instance_name = "i300_" + std::to_string(i);
        std::string location = "results/local_search/i300_cooperative_preferences_a/i300_" + std::to_string(i);
        SSCFLSO instance = Generator::load_instance(file_i, true);
        //gaps(instance, instance_name, location, facility_vector(), 10 * 60 * 1000);
        run(gp, instance, instance_name, location, 10 * 60 * 1000);
    }
}
