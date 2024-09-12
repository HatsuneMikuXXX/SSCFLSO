#include "timer.h"

Timer::Timer() {}

void Timer::start_timer() {
	assert((!running, "Cannot start timer while it's running"));
	running = true;
	pausing = false;
	most_recent_start = clock();
}

void Timer::pause_timer() {
	assert((running, "Cannot pause timer while it's not running."));
	assert((!pausing, "Cannot pause timer while it's already pausing."));
	pausing = true;
	accumulated_cpu_time += clock() - most_recent_start;
}

void Timer::proceed_with_timer() {
	assert((running, "Cannot proceed timer while it's not running."));
	assert((pausing, "Cannot proceed timer while it's not pausing."));
	pausing = false;
	most_recent_start = clock();
}

bool Timer::running_status(){
	return running;
}

double Timer::get_accumulated_cpu_time_in_ms(){
	assert((running, "Timer has not been started yet"));
	assert((pausing, "Timer is not paused!"));
	double precise_result = 1000 * (((double)accumulated_cpu_time)/CLOCKS_PER_SEC);
	return std::ceil(precise_result * 1000) / 1000.0; // round after three decimals
}