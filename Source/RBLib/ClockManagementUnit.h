#ifndef CMU_H
#define CMU_H

#include "Platform.h"

#ifdef EFM32
#include "efm32gg232f1024\ClockManagementUnit.h"
#include "efm32gg990f1024\ClockManagementUnit.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef EFM32
			PlatformEfm32gg232f1024, Efm32gg232f1024::ClockManagementUnit,
			PlatformEfm32gg990f1024, Efm32gg990f1024::ClockManagementUnit
#endif
		>::Result ClockManagementUnitTypes;
	
	// Селектор класса управления портами ввода вывода
	template <Platform p>
	class ClockManagementUnitSelector: public PlatformSelector<ClockManagementUnitTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		ClockManagementUnitSelector()
		{
		
		}
	};

	// Класс управления цифровыми портами ввода-вывода
	typedef ClockManagementUnitSelector<RBLIB_PALTFORM_AUTODETECT> ClockManagementUnit;
}

#endif