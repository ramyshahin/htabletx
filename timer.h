#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>

typedef unsigned long Microseconds;

class Timer
{
private:
	long freq;

public:
	Timer() {
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	}

	Microseconds time(const std::function<void(void)>& f)
	{
		Microseconds tBegin = 0, tEnd = 0;
		Microseconds t = 0;

		if (QueryPerformanceCounter((LARGE_INTEGER*)&tBegin) != 0) {
			f();
			QueryPerformanceCounter((LARGE_INTEGER*)&tEnd);
			t = tEnd - tBegin;
		}
#if 0
		timespec start, end;
		clock_gettime(CLOCK_REALTIME, &start);
		clock_gettime(CLOCK_REALTIME, &end);
#endif
		return t;
	}
}; // Timer

#endif // _TIMER_H

