#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>

typedef unsigned long Microseconds;

class Timer
{
private:
	LARGE_INTEGER freq;

public:
	Timer() {
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	}

	Microseconds time(const std::function<void(void)>& f)
	{
		LARGE_INTEGER tBegin, tEnd;
		Microseconds t = 0;

		if (QueryPerformanceCounter((LARGE_INTEGER*)&tBegin) != 0) {
			f();
			QueryPerformanceCounter((LARGE_INTEGER*)&tEnd);
			t = tEnd.QuadPart - tBegin.QuadPart;
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

