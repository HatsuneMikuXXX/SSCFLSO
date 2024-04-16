#include "libraries.h"

int main(int argc, char* argv[])
{
    // If flag is invalid assume that whole command does not work.
    if (argc <= 1) {
        UI_invalid_msg();
        return 0;
    }
    // Scan
    bool displayedHelpMSG = false;
    bool runAlgo = false;
    bool runAlgoWithGurobi = false;

    Algorithm* algoObject = NULL;
    std::string instance_path = "";
    std::string instance_name = "";
    std::string result_path = "";
    int time_limit = 0;

    for (int i = 1; i < argc; i++) {
        switch (stringFlagtoIntCode(argv[i])) {
        case HELP:
            if (!displayedHelpMSG) {
                UI_help_msg();
                displayedHelpMSG = true;
            }
            break;
        case RUN:
            if (!runAlgo) {
                // Scan next four arguments
                if (i + 4 >= argc) {
                    std::cout << "Too few parameters given for the run command." << std::endl;
                    return -1;
                }
                instance_path = argv[i + 1];
                // Scan name
                {
                    instance_name = argv[i + 1];
                    int pos = 0;
                    std::string token;
                    while ((pos = instance_name.find("/")) != std::string::npos) {
                        token = instance_name.substr(0, pos);
                        instance_name.erase(0, pos + 1);
                    }
                }
                result_path = argv[i + 2];
                try {
                    algoObject = algorithmFactory(stringAlgotoIntCode(argv[i + 3]));
                }
                catch (std::runtime_error e) {
                    std::cerr << "Algorithm \"" << argv[i + 3] << "\"not recognized" << std::endl;
                    return -1;
                }
                try {
                    time_limit = std::stoi(argv[i + 4]);
                }
                catch (std::invalid_argument e) {
                    std::cerr << "Timelimit needs to be numerical" << std::endl;
                    return -1;
                }
                i = i + 4;
            }
            runAlgo = true;
            break;
        case USE_GUROBI_AFTERWARDS:
            runAlgoWithGurobi = true;
            break;
        default:
            UI_invalid_msg();
            return 0;
        }
    }
    // Execute
    {
        if (runAlgo) {
            try {
                SSCFLSO instance = Generator::load_instance(instance_path, true);
                run(algoObject, runAlgoWithGurobi, instance, instance_name, result_path, time_limit);
            }
            catch (std::runtime_error e) {
                std::cout << "Could not find file: " << instance_path << std::endl;
                return -1;
            }
        }
    }
}
