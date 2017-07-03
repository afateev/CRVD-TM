///////////////////////////////////////////////////////////////////////////////
//	Класс управления таймерами
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_H
#define TIMER_H

#include "Platform.h"

#include "ATmega164PA\Timer.h"

namespace Lib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Timer
		>::Result TimerTypes;
	
	// Селектор класса для текущей платформы
	template <Platform p>
	class TimerSelector: public PlatformSelector<TimerTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		TimerSelector()
		{
		
		}
	};

	// Класс для текущей платформы
	typedef TimerSelector<RBLIB_PALTFORM_AUTODETECT> Timer;
}

#endif