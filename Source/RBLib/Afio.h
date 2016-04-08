#ifndef RBLIB_AFIO_H
#define RBLIB_AFIO_H

#include "Platform.h"

#ifdef STM32
#include "stm32\Afio.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef STM32
			PlatformStm32, Stm32::Afio
#endif
		>::Result AfioTypes;
	
	// Селектор класса
	template <Platform p>
	class AfioSelector: public PlatformSelector<AfioTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		AfioSelector()
		{
		
		}
	};

	// Класс
	typedef AfioSelector<RBLIB_PALTFORM_AUTODETECT> Afio;
}

#endif