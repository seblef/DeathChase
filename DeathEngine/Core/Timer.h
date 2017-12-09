
#ifndef _TIMER_H_

#define _TIMER_H_

#include "TSingleton.h"
#include "../Common.h"

namespace Core
{

class Timer : public TSingleton<Timer>
{
	public:

		Timer();
		~Timer() {};

		uint64 GetTimeInMicroS();

	private:

		__int64 start_clock;

		double d_rate_inv;
		__int64 ml_rate;
		__int64 mc_rate;
};

}

#define g_Timer			Core::Timer::GetSingletonRef()

#endif

