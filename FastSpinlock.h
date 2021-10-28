#pragma once

#include <atomic>

class Spinlock
{
public:
	void lock();
	void unlock();

public:
	explicit Spinlock() noexcept = default;

	Spinlock(const Spinlock&) = delete;
	Spinlock& operator=(const Spinlock&) = delete;

private:

	volatile std::atomic_flag lock_flag_ = ATOMIC_FLAG_INIT;
};

