#ifndef RTC_H
#define RTC_H

#include "Platform.h"

#ifdef STM32
#include "stm32\Rtc.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef STM32
		PlatformStm32F4, Stm32::F4::Rtc
#endif
		>::Result RtcTypes;
	
	// Селектор класса управления портами ввода вывода
	template <Platform p>
	class RtcSelector: public PlatformSelector<RtcTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		RtcSelector()
		{
		
		}
	};

	// Класс управления цифровыми портами ввода-вывода
	typedef RtcSelector<RBLIB_PALTFORM_AUTODETECT> Rtc;
}

#endif