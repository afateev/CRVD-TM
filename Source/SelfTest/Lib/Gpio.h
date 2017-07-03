///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_H
#define GPIO_H

#include "Platform.h"

#include "ATmega164PA\Gpio.h"

namespace Lib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Gpio
		>::Result GpioTypes;
	
	// Селектор класса для текущей платформы
	template <Platform p>
	class GpioSelector: public PlatformSelector<GpioTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		GpioSelector()
		{
		
		}
	};

	// Класс для текущей платформы
	typedef GpioSelector<RBLIB_PALTFORM_AUTODETECT> Gpio;
}

#endif