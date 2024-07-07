#include "UI.h"

void start_UI(int argc, char* argv[]) {
    const int maxArgc = 34;
    if (argc <= 1) {
        std::cout << invalid_msg << std::endl;
        return;
    }
    if (argc > maxArgc) {
        std::cout << "Number of arguments is too long. Max number is: " << maxArgc - 2 << std::endl;
        return;
    }

    // Scan Input - Generate Token Array
    TOKEN token_string[maxArgc] = {NO_TOKEN};
    std::vector<Token> stack(0);
    for (int i = 1; i < argc; i++) {
        token_string[i] = scan_arg(stack, argv[i]);
        if (token_string[i] == INVALID) {
            std::cout << invalid_msg << std::endl;
            return;
        }
    }

    // Information to store
    int runFlags = 0;
    bool inputSourceIsDirectory = false;
    std::string inputSource = "";
    std::string outputTarget = "";
    int timelimit = 0;
    Algorithm* algoObjects[number_of_algos] = {}; // Be careful here! Avoid segmentation fault here by querying NULL object.
    int algoObjectsSize = 0;
    bool runAlgoWithGurobi = false;

    // Process token string
    for (int i = 1; i < argc; i++) {
        switch (token_string[i]) {
        case HELP_FLAG:
            std::cout << help_msg << std::endl;
            break;
        case SHOW_ALGOS_FLAG:
            std::cout << algos_msg << std::endl;
            break;
        case SHOW_FORMAT_FLAG:
            std::cout << format_msg << std::endl;
            break;
        case GUROBI_AFTERWARDS_FLAG:
            runAlgoWithGurobi = true;
            break;
        case RUN_FLAG:
            runFlags++;
            if (runFlags >= 2) {
                std::cout << "Please use at most one -run flag. Ignoring this and further -run commands; considering only the first one.";
            }
            break;
        case INPUT_FILE_PATH:
            if (runFlags < 2) {
                inputSource = argv[i];
            }
            break;
        case INPUT_DIRECTORY_PATH:
            if (runFlags < 2) {
                inputSourceIsDirectory = true;
                inputSource = argv[i];
            }
            break;
        case OUTPUT_DIRECTORY_PATH:
            if (runFlags < 2) {
                outputTarget = argv[i];
            }
            break;
        case TIMELIMIT:
            if (runFlags < 2) {
                try {
                    timelimit = std::stoi(argv[i]);
                }
                catch (std::invalid_argument e) {
                    std::cout << "Timelimit could not be converted. Wrong argument." << std::endl;
                }
            }
            break;
        case NO_TOKEN:
            break;
        default:
            // Expect Algorithm token - for safety it's better to enumerate them
            algoObjects[algoObjectsSize] = algorithmFactory(token_string[i]);
            algoObjectsSize++;
            break;
        }
    }

    // Don't execute run command if not required
    if (runFlags > 0) {
        execute_run_command(inputSourceIsDirectory, inputSource, outputTarget, timelimit, algoObjects, algoObjectsSize, runAlgoWithGurobi);
    }
    
}

Algorithm* algorithmFactory(TOKEN algoToken) {
	switch (algoToken) {
	case GUROBI_ALGO:
		return new Gurobi();
	case PREPROCESS_ALGO:
		return new Preprocess();
	case LOCAL_SEARCH_ALGO:
		return new LocalSearch();
	case GREEDY_ALGO:
		return new Greedy();
	case ROUNDING_ALGO:
		return new Rounding();
	case RANDOMIZED_RESTART_ALGO:
		return new RandomizedRestart();
	default:
        return NULL;
	}
}

TOKEN scan_arg(std::vector<Token>& stack, char* argument) {
    std::string input(argument);

    switch (last_token) {
    case RUN_FLAG:
        // Expect a file or directory
        struct stat s;
        if (stat(argument, &s) == 0) {
            if (s.st_mode & S_IFDIR) {
                return INPUT_DIRECTORY_PATH;
            }
            return (s.st_mode & S_IFREG) ? INPUT_FILE_PATH : INVALID;
        }
        return INVALID;
    case INPUT_DIRECTORY_PATH:
    case INPUT_FILE_PATH:
        // Expect a directory
        struct stat s;
        return (stat(argument, &s) == 0 && (s.st_mode & S_IFDIR)) ? OUTPUT_DIRECTORY_PATH : INVALID;
    case OUTPUT_DIRECTORY_PATH:
        // Expect Timelimit
        try {
            std::stoi(input);
            return TIMELIMIT;
        }
        catch (std::invalid_argument e) {
            return INVALID;
        }
        return INVALID;
    case TIMELIMIT:
        // Expect Algorithm
        asa::transform(input, [](unsigned char c) -> char { return std::tolower(c); }); // To lower case
        auto it = std::find_if(std::begin(valid_algorithms), std::end(valid_algorithms), [&input](const std::string& c) -> bool { return input == c; });
        return (it != std::end(valid_algorithms)) ? algo_tokens[std::distance(std::begin(valid_algorithms), it)] : INVALID;
    case GUROBI_ALGO:
    case PREPROCESS_ALGO:
    case LOCAL_SEARCH_ALGO:
    case GREEDY_ALGO:
    case ROUNDING_ALGO:
    case RANDOMIZED_RESTART_ALGO:
        // Expect Algorithm or flag
        {
            asa::transform(input, [](unsigned char c) -> char { return std::tolower(c); }); // To lower case
            auto it = std::find_if(std::begin(valid_algorithms), std::end(valid_algorithms), [&input](const std::string& c) -> bool { return input == c; });
            if (it != std::end(valid_algorithms)) {
                return algo_tokens[std::distance(std::begin(valid_algorithms), it)];
            }
        }
        {
            auto it = std::find_if(std::begin(valid_flags), std::end(valid_flags), [&input](const std::string& c) -> bool { return input == "-" + c || input == "--" + c; });
            return (it != std::end(valid_flags)) ? flag_tokens[std::distance(std::begin(valid_flags), it)] : INVALID;
        }
        return INVALID;
    case NO_TOKEN:
    case HELP_FLAG:
    case SHOW_ALGOS_FLAG:
    case SHOW_FORMAT_FLAG:
    case GUROBI_AFTERWARDS_FLAG:
        // Expect a flag
        auto it = std::find_if(std::begin(valid_flags), std::end(valid_flags), [&input](const std::string& c) -> bool { return input == "-" + c || input == "--" + c; });
        return (it != std::end(valid_flags)) ? flag_tokens[std::distance(std::begin(valid_flags), it)] : INVALID;
    case INVALID:
    default:
        return INVALID;
    }
    return INVALID;
}

void execute_run_command(
    const bool inputSourceIsDirectory,
    const std::string& inputSource,
    const std::string& outputTarget,
    const int timelimit,
    Algorithm* const algoObjects[number_of_algos],
    const int algoObjectsSize,
    const bool runAlgoWithGurobi) {
    // Check if multiple sources are available
    if (inputSourceIsDirectory) {
        DIR* dir; // Directory
        struct dirent* ent; // Read entries in directory 

        if ((dir = opendir(inputSource.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                const std::string filename = ent->d_name;
                if (filename.length() <= 4) { continue; }
                if (filename.compare(filename.length() - 4, 4, ".plc") != 0) { continue; }
                try {
                    const SSCFLSO instance = Generator::load_instance(inputSource + "/" + filename, true);
                    for (int i = 0; i < algoObjectsSize; i++) {
                        if (algoObjects[i] != NULL) {
                            run(instance, filename + "_" + algoObjects[i]->name(), outputTarget, timelimit, algoObjects[i], runAlgoWithGurobi);
                        }
                    }
                }
                catch (std::runtime_error e) {
                    std::cout << "Instance has wrong format" << std::endl;
                }
            }
            closedir(dir);
        }
        else {
            std::cout << "Could not open directory" << std::endl;
        }
    }
    else {
        // No directory, only a single file
        try {
            const SSCFLSO instance = Generator::load_instance(inputSource, true);
            std::string filename = inputSource;
            int pos = 0;
            while ((pos = filename.find("/")) != std::string::npos) {
                filename.erase(0, pos + 1);
            }
            const std::string filename = filename;
            for (int i = 0; i < algoObjectsSize; i++) {
                if (algoObjects[i] != NULL) {
                    run(instance, filename + "_" + algoObjects[i]->name(), outputTarget, timelimit, algoObjects[i], runAlgoWithGurobi);
                }
            }

        }
        catch (std::runtime_error e) {
            std::cout << "Instance has wrong format" << std::endl;
        }
    }
}
