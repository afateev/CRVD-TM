///////////////////////////////////////////////////////////////////////////////
//	Класс управления аппаратными таймерами
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_H
#define TIMER_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\Timer.h"
#include "msp430f2618\Timer.h"
#include "msp430f5340\Timer.h"
//#include "msp430f5342\Timer.h"
#include "msp430f5435a\Timer.h"
#include "msp430f5438a\Timer.h"
#include "msp430f5659\Timer.h"
#endif

#ifdef EFM32
#include "efm32gg\timer.h"
#include "efm32gg\LeTimer.h"
#include "efm32gg\RealTimeCounter.h"
#endif

#ifdef STM32
#include "stm32\timer.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::Timer,
			PlatformMsp430f2618, Msp430f2618::Timer,
			PlatformMsp430f5340, Msp430f5340::Timer,
			//PlatformMsp430f5342, Msp430f5340::Timer,
			PlatformMsp430f5435a, Msp430f5435a::Timer,
			PlatformMsp430f5438a, Msp430f5438a::Timer,
			PlatformMsp430f5659, Msp430f5659::Timer
#endif
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg::Timer,
			PlatformEfm32gg990f1024, Efm32gg::Timer
#endif
#ifdef STM32
			PlatformStm32F1, Stm32::F1::Timer,
			PlatformStm32F4, Stm32::F4::Timer
#endif
		>::Result TimerTypes;
	
	// Селектор класса управления таймерами
	template <Platform p>
	class TimerSelector: public PlatformSelector<TimerTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		TimerSelector()
		{
		
		}
	};

	// Класс управления аппаратными таймерами
	typedef TimerSelector<RBLIB_PALTFORM_AUTODETECT> Timer;
}

#endif