#pragma once

#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

class Spinlock
{
public:

	void lock()
	{
		while (true)
		{
			auto unlocked = !lock_flag_.test_and_set();

			if (unlocked)
				return;

			std::this_thread::sleep_for(1ms);
		}
	}

	void unlock()
	{
		lock_flag_.clear();
	}

public:
	explicit Spinlock() noexcept = default;

	Spinlock(const Spinlock&) = delete;
	Spinlock& operator=(const Spinlock&) = delete;

private:

	volatile std::atomic_flag lock_flag_ = ATOMIC_FLAG_INIT;
};



class Spinlock2
{
public:
	void lock()
	{
		while (lock_flag_.test_and_set())
		{
			std::this_thread::sleep_for(1ms);
		}
	}

	void unlock()
	{
		lock_flag_.clear();
	}

public:
	explicit Spinlock2() noexcept = default;

	Spinlock2(const Spinlock2&) = delete;
	Spinlock2& operator=(const Spinlock2&) = delete;

private:

	volatile std::atomic_flag lock_flag_ = ATOMIC_FLAG_INIT;
};
