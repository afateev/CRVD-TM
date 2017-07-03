///////////////////////////////////////////////////////////////////////////////
//	Класс управления таймерами
//	ATmega164PA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_ATMEGA164PA_H
#define TIMER_ATMEGA164PA_H

#include "../ATmega/Timer.h"
#include "../IdObjectBase.h"

namespace Lib
{
namespace ATmega164PA
{
	// Платформозависимый набор таймеров
	class Timer
	{
	public:
		typedef ATmega::Timer::Timer8bitPwm<1, 0x44, 0x45, 0x46, 0x47, 0x48, 0x6E, 0x35> Timer0;
		typedef ATmega::Timer::Timer8bitPwm<3, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0x70, 0x37> Timer2;
	};
}
}

#endif