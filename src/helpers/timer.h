#ifndef TIMER_H
#define TIMER_H

#include "../common.h"

class Timer {
public:
	Timer(int time_limit_in_ms);
	void start_timer();
	void pause_timer();
	void proceed_with_timer();
	double get_remaining_time() const;
	double get_elapsed_time() const;
	bool in_time() const;
	bool running_status() const;
	int time_limit() const;
private:
	std::chrono::system_clock::time_point most_recent_start;
	int total_time;
	double elapsed_time_until_last_pause{0};
	bool running{false};
	bool pausing{false};
};
#endif