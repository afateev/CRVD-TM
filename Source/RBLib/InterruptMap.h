///////////////////////////////////////////////////////////////////////////////
//	Класс управления вектором прерываний
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_H
#define INTERRUPTMAP_H

#include "Platform.h"
#ifdef MSP430
#include "msp430f1611\InterruptMap.h"
#include "msp430f2618\InterruptMap.h"
#include "msp430f5340\InterruptMap.h"
//#include "msp430f5342\InterruptMap.h"
#include "msp430f5435a\InterruptMap.h"
#include "msp430f5438a\InterruptMap.h"
#include "msp430f5659\InterruptMap.h"
#endif

#ifdef EFM32
#include "efm32gg\InterruptMap.h"
#endif

#ifdef STM32
#include "stm32\InterruptMap.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::InterruptMap,
			PlatformMsp430f2618, Msp430f2618::InterruptMap,
      PlatformMsp430f5340, Msp430f5340::InterruptMap,	
			//PlatformMsp430f5342, Msp430f5342::InterruptMap,
			PlatformMsp430f5435a, Msp430f5435a::InterruptMap,
			PlatformMsp430f5438a, Msp430f5438a::InterruptMap,
			PlatformMsp430f5659, Msp430f5659::InterruptMap
#endif
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg::InterruptMap,
			PlatformEfm32gg990f1024, Efm32gg::InterruptMap
#endif
#ifdef STM32
			PlatformStm32F1, Stm32::F1::InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>,
			PlatformStm32F4, Stm32::F4::InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>
#endif
		>::Result InterruptMapTypes;
	
	// Селектор класса вектора прерываний
	template <Platform p>
    class InterruptMapSelector: public PlatformSelector<InterruptMapTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию
		InterruptMapSelector()
		{
		}
	};

	// Класс управления вектором прерываний
	typedef InterruptMapSelector<RBLIB_PALTFORM_AUTODETECT> InterruptMap;
}

#endif
