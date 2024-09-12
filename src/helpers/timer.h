#ifndef TIMER_H
#define TIMER_H
#include <cassert>
#include <time.h>
#include <cmath>

const int ONE_MINUTE = 60000;

class Timer {
public:
	Timer();
	void start_timer();
	void pause_timer();
	void proceed_with_timer();
	double get_accumulated_cpu_time_in_ms();
	bool running_status();
private:
	clock_t most_recent_start{0};
	clock_t accumulated_cpu_time{0};
	bool running{false};
	bool pausing{false};
};
#endif