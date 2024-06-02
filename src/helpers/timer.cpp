#include "timer.h"

Timer::Timer(int time_limit_in_ms) : total_time(time_limit_in_ms) {}

void Timer::start_timer() {
	assert(!this->running, "Cannot start timer while it's running");
	this->running = true;
	this->pausing = false;
	this->most_recent_start = std::chrono::system_clock::now();
}

double Timer::get_remaining_time() const {
	assert(this->running, "Cannot compute remaining time while timer is not running.");
	return this->total_time - get_elapsed_time();
}

double Timer::get_elapsed_time() const {
	assert(this->running, "Cannot compute elapsed time while timer is not running.");
	if (this->pausing) {
		return this->elapsed_time_until_last_pause;
	}
	else {
		double elapsed_time_since_last_pause = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - this->most_recent_start).count();
		return this->elapsed_time_until_last_pause + elapsed_time_since_last_pause;
	}
}

void Timer::pause_timer() {
	assert(this->running, "Cannot pause timer while it's not running.");
	assert(!this->pausing, "Cannot pause timer while it's already pausing.");
	this->pausing = true;
	this->elapsed_time_until_last_pause += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - this->most_recent_start).count();
}

void Timer::proceed_with_timer() {
	assert(this->running, "Cannot proceed timer while it's not running.");
	assert(this->pausing, "Cannot proceed timer while it's not pausing.");
	this->pausing = false;
	this->most_recent_start = std::chrono::system_clock::now();
}

bool Timer::in_time() const {
	return (running) ? this->get_remaining_time() >= 0 : true;
}

bool Timer::running_status() const {
	return running;
}

int Timer::time_limit() const {
	return this->total_time;
}