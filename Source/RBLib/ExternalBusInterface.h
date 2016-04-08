#ifndef EBI_H
#define EBI_H

#include "Platform.h"

#ifdef EFM32
//#include "efm32gg232f1024\ExternalBusInterface.h"
#include "efm32gg990f1024\ExternalBusInterface.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef EFM32
			//PlatformEfm32gg232f1024, Efm32gg232f1024::ExternalBusInterface,
			PlatformEfm32gg990f1024, Efm32gg990f1024::ExternalBusInterface
#endif
		>::Result ExternalBusInterfaceTypes;
	
	// Селектор класса управления портами ввода вывода
	template <Platform p>
	class ExternalBusInterfaceSelector: public PlatformSelector<ExternalBusInterfaceTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		ExternalBusInterfaceSelector()
		{
		
		}
	};

	// Класс управления цифровыми портами ввода-вывода
	typedef ExternalBusInterfaceSelector<RBLIB_PALTFORM_AUTODETECT> ExternalBusInterface;
}

#endif