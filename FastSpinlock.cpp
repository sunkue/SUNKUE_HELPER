#include <chrono>
#include <thread>
#include "FastSpinlock.h"

using namespace std::chrono_literals;

void Spinlock::lock()
{
	while (true)
	{
		if (false == lock_flag_.test_and_set())
			return;

		std::this_thread::sleep_for(1ms);
	}
}

void Spinlock::unlock()
{
	lock_flag_.clear();
}