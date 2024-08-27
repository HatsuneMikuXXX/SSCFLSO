#include "UI/UI.h"
#include "unit_testing_framework/unit_test.h"

int main(int argc, char* argv[])
{
    std::cout << "Starting SSCFLSO Application" << std::endl;
    srand(17 + 0);
    start_UI(argc, argv);
    return 0;
}
