///////////////////////////////////////////////////////////////////////////////
//	Класс управления флэш памятью
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_H
#define FLASHMEMORYCONTROLLER_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\FlashMemoryController.h"
#include "msp430f2618\FlashMemoryController.h"
#include "msp430f5340\FlashMemoryController.h"
//#include "msp430f5342\FlashMemoryController.h"
#include "msp430f5435a\FlashMemoryController.h"
#include "msp430f5438a\FlashMemoryController.h"
#include "msp430f5659\FlashMemoryController.h"
#endif

#ifdef STM32
#include "stm32\FlashMemoryController.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::FlashMemoryController,
			PlatformMsp430f2618, Msp430f2618::FlashMemoryController,
			PlatformMsp430f5340, Msp430f5340::FlashMemoryController,
			//PlatformMsp430f5342, Msp430f5342::FlashMemoryController,
			PlatformMsp430f5435a, Msp430f5435a::FlashMemoryController,
			PlatformMsp430f5438a, Msp430f5438a::FlashMemoryController,
			PlatformMsp430f5659, Msp430f5659::FlashMemoryController
#endif
#ifdef STM32
			PlatformStm32, Stm32::FlashMemoryController
#endif
		>::Result FlashMemoryControllerTypes;
	
	// Селектор класса управления флэш памятью
	template <Platform p>
	class FlashMemoryControllerSelector: public PlatformSelector<FlashMemoryControllerTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		FlashMemoryControllerSelector()
		{
		
		}
	};

	//Класс управления флэш памятью
	typedef FlashMemoryControllerSelector<RBLIB_PALTFORM_AUTODETECT> FlashMemoryController;
}

#endif
