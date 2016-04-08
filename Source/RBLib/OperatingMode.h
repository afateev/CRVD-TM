///////////////////////////////////////////
//
// Управление режимами энергопотребления
//
///////////////////////////////////////////

#ifndef OPERATING_MODE_H
#define OPERATING_MODE_H

#include "Platform.h"

#ifdef __ICC430__
#include "msp430f1611\OperatingMode.h"
#include "msp430f2618\OperatingMode.h"
#include "msp430f5340\OperatingMode.h"
//#include "msp430f5342\OperatingMode.h"
#include "msp430f5435a\OperatingMode.h"
#include "msp430f5438a\OperatingMode.h"
#include "msp430f5659\OperatingMode.h"
#endif

namespace Rblib
{
	// Селекстор платформа / пространство имём
	typedef MakePlatformTypelist<
#ifdef __ICC430__
			PlatformMsp430f1611, Msp430f1611::OperatingMode,
			PlatformMsp430f2618, Msp430f2618::OperatingMode,
      PlatformMsp430f5340, Msp430f5340::OperatingMode,
			//PlatformMsp430f5342, Msp430f5342::OperatingMode,
			PlatformMsp430f5435a, Msp430f5435a::OperatingMode,
			PlatformMsp430f5438a, Msp430f5438a::OperatingMode,
			PlatformMsp430f5659, Msp430f5659::OperatingMode
#endif
		>::Result OperatingModeTypes;

	// Селектор класса управления режимами работы (параметры энергосбережения)
	template <Platform p>
	class OperatingModeSelector: public PlatformSelector<OperatingModeTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		OperatingModeSelector()
		{
		}
		OperatingModeSelector(const OperatingModeSelector&)
		{
		}
		~OperatingModeSelector()
		{
		}
	};

	// Класс управления режимами работы
	typedef OperatingModeSelector<RBLIB_PALTFORM_AUTODETECT> OperatingMode;
}

#endif // OPERATING_MODE_H
