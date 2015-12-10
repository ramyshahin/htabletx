#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>

typedef double Microseconds;

class Timer
{
private:
	double freq;

public:
	Timer() {
		LARGE_INTEGER fr;
		QueryPerformanceFrequency((LARGE_INTEGER*)&fr);
		freq = ((double)fr.QuadPart) / 1000000.0f;
	}

	Microseconds time(const std::function<void(void)>& f)
	{
		LARGE_INTEGER tBegin, tEnd;
		Microseconds t = 0;

		if (QueryPerformanceCounter((LARGE_INTEGER*)&tBegin) != 0) {
			f();
			QueryPerformanceCounter((LARGE_INTEGER*)&tEnd);
			t = (tEnd.QuadPart - tBegin.QuadPart) / freq;
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

