#ifndef RCC_H
#define RCC_H

#include "Platform.h"

#ifdef STM32
#include "stm32\ResetAndClockControl.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32, Stm32::ResetAndClockControl
#endif
		>::Result ResetAndClockControlTypes;
	
	// Селектор класса управления портами ввода вывода
	template <Platform p>
	class ResetAndClockControlSelector: public PlatformSelector<ResetAndClockControlTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		ResetAndClockControlSelector()
		{
		
		}
	};

	// Класс управления цифровыми портами ввода-вывода
	typedef ResetAndClockControlSelector<RBLIB_PALTFORM_AUTODETECT> ResetAndClockControl;
}

#endif