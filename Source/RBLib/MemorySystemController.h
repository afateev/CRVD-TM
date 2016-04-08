#ifndef MEMORYSYSTEMCONTROLLER_H
#define MEMORYSYSTEMCONTROLLER_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg\MemorySystemController.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg::MemorySystemController,
			PlatformEfm32gg990f1024, Efm32gg::MemorySystemController
#endif
		>::Result MemorySystemControllerTypes;
	
	// Селектор класса
	template <Platform p>
    class MemorySystemControllerSelector: public PlatformSelector<MemorySystemControllerTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию
		MemorySystemControllerSelector()
		{
		}
	};

	// Класс
	typedef MemorySystemControllerSelector<RBLIB_PALTFORM_AUTODETECT> MemorySystemController;
}

#endif