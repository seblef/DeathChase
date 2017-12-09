
#include "Timer.h"
#include <windows.h>

namespace Core
{

Timer::Timer()
{
	__int64 rate;

	if(!QueryPerformanceFrequency((LARGE_INTEGER *)&rate))
		return;

	if(!rate)
		return;

	d_rate_inv=1.0f / (double)rate;
	ml_rate=rate / 1000;
	mc_rate=rate / 1000000;

	QueryPerformanceFrequency((LARGE_INTEGER *)&start_clock);
}

uint64 Timer::GetTimeInMicroS()
{
	__int64 current_time;

	QueryPerformanceCounter((LARGE_INTEGER *)&current_time);

	return (current_time -start_clock)/ mc_rate;
}

}