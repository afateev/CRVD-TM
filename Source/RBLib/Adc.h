///////////////////////////////////////////////////////////////////////////////
//	Класс управления АЦП
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_H
#define ADC_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\Adc.h"
#include "msp430f2618\Adc.h"
#include "msp430f5340\Adc.h"
#include "msp430f5342\Adc.h"
#include "msp430f5435a\Adc.h"
#include "msp430f5438a\Adc.h"
#include "msp430f5659\Adc.h"
#endif

#ifdef EFM32
#include "efm32gg232f1024\Adc.h"
#include "efm32gg990f1024\Adc.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::Adc,
			PlatformMsp430f2618, Msp430f2618::Adc,
			PlatformMsp430f5340, Msp430f5340::Adc,
			PlatformMsp430f5342, Msp430f5342::Adc,
			PlatformMsp430f5435a, Msp430f5435a::Adc,
			PlatformMsp430f5438a, Msp430f5438a::Adc,
			PlatformMsp430f5659, Msp430f5659::Adc
#endif
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::Adc,
			PlatformEfm32gg990f1024, Efm32gg990f1024::Adc
#endif
		>::Result AdcTypes;
	
	// Селектор класса управления сторожевым таймером
	template <Platform p>
	class AdcSelector: public PlatformSelector<AdcTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		AdcSelector()
		{
		
		}
	};

	// Класс управления АЦП
	typedef AdcSelector<RBLIB_PALTFORM_AUTODETECT> Adc;
}

#endif