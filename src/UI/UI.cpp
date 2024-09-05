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
    std::vector<TOKEN> stack(0);
    for (int i = 1; i < argc; i++) {
        token_string[i] = scan_arg(stack, argv[i]);
        if (token_string[i] == INVALID) {
            std::cout << std::to_string(i);
            switch (i) {
            case 1:
            case 21:
            case 31:
                std::cout << "st "; break;
            case 2:
            case 22:
            case 32:
                std::cout << "nd "; break;
            case 3:
            case 23:
            case 33:
                std::cout << "rd "; break;
            default:
                std::cout << "th "; break;
            }
            std::cout << "argument \"" << argv[i] << "\" is invalid or caused issues.";
            std::cout << invalid_msg;
            return;
        }
    }

    // Information to store
    int runFlags = 0;
    bool inputSourceIsDirectory = false;
    std::string inputSource = "";
    std::string outputTarget = "";
    int timelimit = 0;
    Algorithm* algoObjects[maxArgc] = {}; // Be careful here! Avoid segmentation fault here by querying NULL object.
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
                std::cout << "Please use at most one -run flag. Ignoring this and further -run commands; considering only the first one." << std::endl;
            }
            if (i + 4 >= argc) {
                std::cout << "Too few parameters to run the command" << std::endl;
                return;
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
        case GRASP_ALGO:
            // Special case because it involves a numeric parameter
            if (i + 1 >= argc) {
                std::cout << "No parameter given for GRASP algo. Skipping over." << std::endl;
                algoObjects[algoObjectsSize++] = NULL;
            }
            if (token_string[++i] == TYPE_DOUBLE_VALUE) {
                try {
                    double RCL_percentile = std::stod(argv[i]);
                    if (RCL_percentile < 0) {
                        RCL_percentile = 0;
                    }
                    else if (RCL_percentile > 1) {
                        RCL_percentile = 1;
                    }
                    std::cout << "GRASP algorithm recognized" << std::endl;
                    algoObjects[algoObjectsSize++] = new GRASP(RCL_percentile);
                }
                catch (std::invalid_argument e) {
                    std::cout << "Numeric parameter for GRASP not recognized." << std::endl;
                }
            }
            else {
                algoObjects[algoObjectsSize++] = NULL;
                std::cout << "GRASP parameters not recognized" << std::endl;
            }
            break;
        case GUROBI_ALGO:
        case LAGRANGIAN_RELAXATION_ALGO:
        case PREPROCESS_ALGO:
        case ROUNDING_ALGO:
        case SEMI_LAGRANGIAN_RELAXATION_ALGO:
        case LOCAL_SEARCH_ALGO:
        case COMPOSITE_ALGO:
            algoObjects[algoObjectsSize++] = algorithmFactory(token_string, i, argc);
            break;
        default:
            break;
        }
    }
    
    if (runFlags > 0) {
        execute_run_command(inputSourceIsDirectory, inputSource, outputTarget, timelimit, algoObjects, algoObjectsSize, runAlgoWithGurobi);
    }
}

Algorithm* algorithmFactory(TOKEN* token_string, int& current_index, int max_index) {
	switch (token_string[current_index]) {
    case GUROBI_ALGO:
        std::cout << "Gurobi algorithm recognized." << std::endl;
        return new Gurobi();
    case LAGRANGIAN_RELAXATION_ALGO:
        std::cout << "LR algorithm recognized." << std::endl;
        return new LagrangianRelaxation();
    case PREPROCESS_ALGO:
        std::cout << "Preprocess algorithm recognized." << std::endl;
        return new Preprocess();
    case ROUNDING_ALGO:
        std::cout << "Rounding algorithm recognized." << std::endl;
        return new Rounding();
    case SEMI_LAGRANGIAN_RELAXATION_ALGO:
        if (current_index + 1 >= max_index) {
            std::cout << "No parameter given for semi lagrangian relaxation. Skipping over." << std::endl;
            return NULL;
        }
        switch (token_string[++current_index]) {
        case TYPE_BOOLEAN_TRUE:
            std::cout << "SLR with homogeneous weights algorithm recognized." << std::endl;
            return new SemiLagrangianRelaxation(true);
        case TYPE_BOOLEAN_FALSE:
            std::cout << "SLR algorithm recognized." << std::endl;
            return new SemiLagrangianRelaxation(false);
        }
        break;
    case LOCAL_SEARCH_ALGO: {
        if (current_index + 2 >= max_index) {
            std::cout << "Not enough parameter given for local search. Skipping over." << std::endl;
            return NULL;
        }
        TOKEN t1 = token_string[++current_index];
        TOKEN t2 = token_string[++current_index];
        if (t1 != NEXTNEIGHBOR_BEST_PARAM && t1 != NEXTNEIGHBOR_FIRST_PARAM) {
            TOKEN tmp = t1;
            t1 = t2;
            t2 = tmp;
        }
        switch (t1) {
        case NEXTNEIGHBOR_BEST_PARAM:
            switch (t2) {
            case SOLBY_GIVEN_PARAM:
                std::cout << "Local search (best neighbor) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::GIVEN, LocalSearch::BEST);
            case SOLBY_PREPROCESS_PARAM:
                std::cout << "Local search (best neighbor, preprocessing) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::PREPROCESS, LocalSearch::BEST);
            case SOLBY_RANDOM_PARAM:
                std::cout << "Local search (best neighbor, randomization + seek) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::RANDOM, LocalSearch::BEST);
            case SOLBY_RANDOM_RESTART_PARAM:
                std::cout << "Local search (best neighbor, randomization) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::RANDOM_RESTART, LocalSearch::BEST);
            }
            break;
        case NEXTNEIGHBOR_FIRST_PARAM:
            switch (t2) {
            case SOLBY_GIVEN_PARAM:
                std::cout << "Local search (first feasible neighbor) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::GIVEN, LocalSearch::FIRST);
            case SOLBY_PREPROCESS_PARAM:
                std::cout << "Local search (first feasible neighbor, preprocess) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::PREPROCESS, LocalSearch::FIRST);
            case SOLBY_RANDOM_PARAM:
                std::cout << "Local search (first feasible neighbor, randomization + seek) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::RANDOM, LocalSearch::FIRST);
            case SOLBY_RANDOM_RESTART_PARAM:
                std::cout << "Local search (first feasible neighbor, randomization) algorithm recognized." << std::endl;
                return new LocalSearch(LocalSearch::RANDOM_RESTART, LocalSearch::FIRST);
            }
            break;
        }
        break;
    }
    case COMPOSITE_ALGO: {
        int number_of_algos = 0;
        std::vector<Algorithm*> algorithms(0);
        std::function<bool(TOKEN)> part_of_composition([](const TOKEN token) -> bool {
            // Either algo (not composition) or param
            bool b1 = asa::any_of(algo_tokens, [token](const TOKEN algo_token) -> bool { return token == algo_token; });
            bool b2 = asa::any_of(init_param_tokens, [token](const TOKEN init_token) -> bool {return  token == init_token; });
            bool b3 = asa::any_of(next_param_tokens, [token](const TOKEN next_token) -> bool { return token == next_token; });

            return (token != COMPOSITE_ALGO) && (b1 || b2 || b3);
            });
        while (current_index + 1 < max_index && part_of_composition(token_string[++current_index])) {
            number_of_algos++;
            algorithms.push_back(algorithmFactory(token_string, current_index, max_index));
        }
        std::cout << "Composition algorithm with " << number_of_algos << " algorithms recognized." << std::endl;
        return new Composite(algorithms);
    }
        
	default:
        std::cout << "Algorithm not recognized." << std::endl;
        return NULL;
	}
    return NULL;
}

TOKEN scan_arg(std::vector<TOKEN>& stack, char* argument) {
    std::string input(argument);
    asa::transform(input, [](unsigned char c) -> char { return std::tolower(c); }); // To lower case

    std::function<TOKEN()> find_flag_token([&input]() -> TOKEN {
        auto it = std::find_if(std::begin(valid_flags), std::end(valid_flags), [&input](const std::string& c) -> bool { return input == "-" + c || input == "--" + c; });
        return  (it != std::end(valid_flags)) ? flag_tokens[std::distance(std::begin(valid_flags), it)] : INVALID;
    });
    std::function<TOKEN()> find_algo_token([&input]() -> TOKEN {
        auto it = std::find_if(std::begin(valid_algorithms), std::end(valid_algorithms), [&input](const std::string& c) -> bool { return input == c; });
        return  (it != std::end(valid_algorithms)) ? algo_tokens[std::distance(std::begin(valid_algorithms), it)] : INVALID;
    });
    std::function<TOKEN()> find_init_param_token([&input]() -> TOKEN {
        auto it = std::find_if(std::begin(valid_init_params), std::end(valid_init_params), [&input](const std::string& c) -> bool { return input == c; });
        return  (it != std::end(valid_init_params)) ? init_param_tokens[std::distance(std::begin(valid_init_params), it)] : INVALID;
    });
    std::function<TOKEN()> find_next_param_token([&input]() -> TOKEN {
        auto it = std::find_if(std::begin(valid_next_params), std::end(valid_next_params), [&input](const std::string& c) -> bool { return input == c; });
        return  (it != std::end(valid_next_params)) ? next_param_tokens[std::distance(std::begin(valid_next_params), it)] : INVALID;
    });

    TOKEN token;

    switch (stack.size()) {
    case 0:
        // Expect a flag
        token = find_flag_token();
        if (token == RUN_FLAG) {
            stack.push_back(RUN_FLAG);
        }
        return token;
    case 1:
        switch (stack[0]) {
        case RUN_FLAG:
        {
            // Expect a directory or file
            stack.clear();
            struct stat s;
            if (stat(argument, &s) == 0) {
                if (s.st_mode & S_IFDIR) {
                    stack.push_back(INPUT_DIRECTORY_PATH);
                    return INPUT_DIRECTORY_PATH;
                }
                if (s.st_mode & S_IFREG) {
                    stack.push_back(INPUT_FILE_PATH);
                    return INPUT_FILE_PATH;
                }
                return INVALID;
            }
            return INVALID;
        }
        case INPUT_DIRECTORY_PATH:
        case INPUT_FILE_PATH:
            stack.clear();
            // Expect a directory
            struct stat s;
            if (stat(argument, &s) == 0 && (s.st_mode & S_IFDIR)) {
                stack.push_back(OUTPUT_DIRECTORY_PATH);
                return OUTPUT_DIRECTORY_PATH;
            }
            return INVALID;
        case OUTPUT_DIRECTORY_PATH:
            // Expect a time limit
            stack.clear();
            try {
                int dontcare = std::stoi(input);
                stack.push_back(TIMELIMIT);
                return TIMELIMIT;
            }
            catch (std::invalid_argument e) {
                return INVALID;
            }
            return INVALID;
        case TIMELIMIT:
            // Expect an algorithm
            stack.clear();
            token = find_algo_token();
            stack.push_back(token);
            return token;
        case GUROBI_ALGO:
        case LAGRANGIAN_RELAXATION_ALGO:
        case PREPROCESS_ALGO:
        case ROUNDING_ALGO:
            // Expect another Algorithm or flag
            stack.clear();
            {
                // Algo
                token = find_algo_token();
                if (token != INVALID) {
                    stack.push_back(token);
                    return token;
                }
            }
            {
                // Flag
                token = find_flag_token();
                if (token == RUN_FLAG) {
                    // Which will later be ignored anyways because we only accept one run command currently
                    stack.push_back(RUN_FLAG);
                }
                return token;
            }
            return INVALID;
        case SEMI_LAGRANGIAN_RELAXATION_ALGO:
            // Expect true or false
            stack.clear();
            if (input == "true" || input == "1") {
                // Place any Algo Token without parameters because we have now seen all params
                stack.push_back(PREPROCESS_ALGO);
                return TYPE_BOOLEAN_TRUE;
            }
            else if (input == "false" || input == "0") {
                // Place any Algo Token without parameters because we have now seen all params
                stack.push_back(PREPROCESS_ALGO);
                return TYPE_BOOLEAN_FALSE;
            }
            return INVALID;
        case GRASP_ALGO:
            // Expect a value in [0,1]
            stack.clear();
            try {
                double dontcare = std::stod(input);
                // Place any Algo Token without parameters because we have now seen all params
                stack.push_back(PREPROCESS_ALGO);
                return TYPE_DOUBLE_VALUE;
            }
            catch (std::invalid_argument e) {
                return INVALID;
            }
            return INVALID;
        case LOCAL_SEARCH_ALGO:
            // Expect an init or next parameter
        {
            // Init
            token = find_init_param_token();
            if (token != INVALID) {
                stack.push_back(token);
                return token;
            }
        }
        {
            // Next
            token = find_next_param_token();
            stack.push_back(token);
            return token;
        }
        return INVALID;
        case COMPOSITE_ALGO:
            // Expect Algo
            {
                // Algo
                token = find_algo_token();
                stack.push_back(token);
                return token;
            }
        }
    case 2:
        switch (stack[0]) {
        case LOCAL_SEARCH_ALGO:
            if (stack[1] == NEXTNEIGHBOR_FIRST_PARAM || stack[1] == NEXTNEIGHBOR_BEST_PARAM) {
                // Expect init parameter
                token = find_init_param_token();
                stack.clear();
                // Place any Algo Token without parameters because we have now seen all params
                
                stack.push_back(PREPROCESS_ALGO);
                return token;
            }
            else {
                // Expect next parameter
                token = find_next_param_token();
                stack.clear();
                // Place any Algo Token without parameters because we have now seen all params
                stack.push_back(PREPROCESS_ALGO);
                return token;
            }
            return INVALID;
        case COMPOSITE_ALGO:
            // Check if we need to expect params first.
            if (stack.back() == LOCAL_SEARCH_ALGO) {
                {
                    // Init
                    token = find_init_param_token();
                    if (token != INVALID) {
                        stack.push_back(token);
                        return token;
                    }
                }
                {
                    // Next
                    token = find_next_param_token();
                    stack.push_back(token);
                    return token;
                }
            }
            // Expect another Algo
            token = find_algo_token();
            stack.push_back(token);
            return token;
        }
        break;
    default:
        if (stack[0] != COMPOSITE_ALGO) {
            return INVALID;
        }   
        // Check if we need to expect params first.
        if (stack.rbegin()[1] == LOCAL_SEARCH_ALGO) {
            if (stack.back() == NEXTNEIGHBOR_FIRST_PARAM || stack.back() == NEXTNEIGHBOR_BEST_PARAM) {
                // Expect init parameter
                token = find_init_param_token();
                stack.push_back(token);
                return token;
            }
            else {
                // Expect next parameter
                token = find_next_param_token();
                stack.push_back(token);
                return token;
            }
        }
        if (stack.back() == LOCAL_SEARCH_ALGO) {
            {
                // Init
                token = find_init_param_token();
                if (token != INVALID) {
                    stack.push_back(token);
                    return token;
                }
            }
            {
                // Next
                token = find_next_param_token();
                stack.push_back(token);
                return token;
            }
        }
        // Expect Algo or flag
        {
            // Algo
            token = find_algo_token();
            if (token != INVALID) {
                stack.push_back(token);
                return token;
            }
        }
        {
            stack.clear();
            // Flag
            token = find_flag_token();
            if (token == RUN_FLAG) {
                // Which will later be ignored anyways because we only accept one run command currently
                stack.push_back(RUN_FLAG);
            }
            return token;
        }
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

    // C++ 17
    if (inputSourceIsDirectory) {
        // Determine total number of files
        double tot = 0;
        for (const auto& entry : std::filesystem::directory_iterator(inputSource)) {
            std::string filename = entry.path().string();
            std::replace(filename.begin(), filename.end(), '\\', '/');
            while (filename.find("/") != std::string::npos) {
                filename = filename.substr(filename.find("/") + 1);
            }
            if (filename.length() <= 4) { continue; }
            if (filename.compare(filename.length() - 4, 4, ".plc") != 0) { continue; }
            tot++;
        }

        // Actual Execution
        double cnt = 0;
        for (const auto& entry : std::filesystem::directory_iterator(inputSource)) {
            std::string filename = entry.path().string();
            std::replace(filename.begin(), filename.end(), '\\', '/');
            while (filename.find("/") != std::string::npos) {
                filename = filename.substr(filename.find("/") + 1);
            }
            // Scan all .plc files and ignore the rest
            if (filename.length() <= 4) { continue; }
            if (filename.compare(filename.length() - 4, 4, ".plc") != 0) { continue; }
            try {
                const SSCFLSO instance = Generator::load_instance(inputSource + "/" + filename, true);
                for (int i = 0; i < algoObjectsSize; i++) {
                    if (algoObjects[i] != NULL) {
                        double val = 100 * (++cnt) / (tot);
                        val = std::ceil(val * 100.0) / 100.0;
                        std::cout << "Progress: " << val << "% (" << cnt << "/" << tot << ").";
                        std::cout << " Running " << algoObjects[i]->name();
                        if (runAlgoWithGurobi) {
                            std::cout << " + Gurobi";
                        }
                        std::cout << " on " << filename << std::endl;
                        run(instance, filename, outputTarget, timelimit, algoObjects[i], runAlgoWithGurobi);
                    }
                }
            }
            catch (std::runtime_error e) {
                std::cout << "The file " << filename << " has wrong format or couldn't be found." << std::endl;
            }
        }
    }
    else {
        // No directory, only a single file
        try {
            const SSCFLSO instance = Generator::load_instance(inputSource, true);
            std::string filename = inputSource;
            size_t pos = 0;
            while ((pos = filename.find("/")) != std::string::npos) {
                filename.erase(0, pos + 1);
            }
            for (int i = 0; i < algoObjectsSize; i++) {
                if (algoObjects[i] != NULL) {
                    run(instance, filename, outputTarget, timelimit, algoObjects[i], runAlgoWithGurobi);
                }
            }

        }
        catch (std::runtime_error e) {
            std::cout << "Instance has wrong format or couldn't be found." << std::endl;
        }
    }
}
