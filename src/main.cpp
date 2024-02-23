#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace std;

// stop flag
bool stopfoo;

// function to run until stopped
void foo()
{
    while( ! stopfoo )
    {
        // replace with something useful
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "still working!\n";
    }
    std::cout << "stopped\n";
}

// function to call a top after 5 seconds
void timer()
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    stopfoo = true;
}

int main()
{
    // initialize stop flag
    stopfoo = false;
    // start timer in its own thread
    std::thread t(timer);
    // start worker in main thread
    foo();
    return 0;
}

/*
typedef std::vector<int> solution;
typedef std::function<solution(SSCFLSO&)> algorithm;

int main()
{
    bool success = TESTER::run_all_tests();
    return success;
}
*/