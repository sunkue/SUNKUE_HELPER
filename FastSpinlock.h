#pragma once

#include <atomic>

class Spinlock
{
public:
	Spinlock() noexcept
	{
		timeBeginPeriod(1);
	}

	Spinlock(const Spinlock&) = delete;
	Spinlock& operator=(const Spinlock&) = delete;

public:
	void lock();
	void unlock();

private:

	volatile std::atomic_bool lock_flag_ = 1;
};

