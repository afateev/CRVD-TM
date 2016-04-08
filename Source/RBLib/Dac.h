///////////////////////////////////////////////////////////////////////////////
//	Класс управления ЦАП
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef DAC_H
#define DAC_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg232f1024\Dac.h"
#include "efm32gg990f1024\Dac.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::Dac,
			PlatformEfm32gg990f1024, Efm32gg990f1024::Dac
#endif
		>::Result DacTypes;
	
	// Селектор класса
	template <Platform p>
	class DacSelector: public PlatformSelector<DacTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		DacSelector()
		{
		
		}
	};

	// Класс управления ЦАП
	typedef DacSelector<RBLIB_PALTFORM_AUTODETECT> Dac;
}

#endif