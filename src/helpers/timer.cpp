#include "timer.h"

Timer::Timer(int time_limit_in_ms) : total_time(time_limit_in_ms) {}

void Timer::start_timer() {
	assert(!running, "Cannot start timer while it's running");
	running = true;
	pausing = false;
	most_recent_start = std::chrono::system_clock::now();
}

double Timer::get_remaining_time() const {
	assert(running, "Cannot compute remaining time while timer is not running.");
	return total_time - get_elapsed_time();
}

double Timer::get_elapsed_time() const {
	assert(running, "Cannot compute elapsed time while timer is not running.");
	if (pausing) {
		return elapsed_time_until_last_pause;
	}
	else {
		double elapsed_time_since_last_pause = double(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - most_recent_start).count());
		return elapsed_time_until_last_pause + elapsed_time_since_last_pause;
	}
}

void Timer::pause_timer() {
	assert(running, "Cannot pause timer while it's not running.");
	assert(!pausing, "Cannot pause timer while it's already pausing.");
	pausing = true;
	elapsed_time_until_last_pause += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - most_recent_start).count();
}

void Timer::proceed_with_timer() {
	assert(running, "Cannot proceed timer while it's not running.");
	assert(pausing, "Cannot proceed timer while it's not pausing.");
	pausing = false;
	most_recent_start = std::chrono::system_clock::now();
}

bool Timer::in_time() const {
	return (running) ? get_remaining_time() >= 0 : true;
}

bool Timer::running_status() const {
	return running;
}

int Timer::time_limit() const {
	return total_time;
}