#include "UI/UI.h"
#include "unit testing framework/unit_test.h"

int main(int argc, char* argv[])
{
    srand(17);
    if (argc < 2) {
        start_unit_testing(argc);
    }
    else {
        start_UI(argc, argv);
    }
    return 0;
}
