#include <windows.h>
#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "FastSpinlock.h"

void Spinlock::lock()
{
	for (int nloops = 0; ; nloops++)
	{
		if (lock_flag_.exchange(1) == 0)
			return;
	
		Sleep((DWORD)min(10, nloops));
	}
}

void Spinlock::unlock()
{
	lock_flag_.exchange(0);
}