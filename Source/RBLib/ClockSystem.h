///////////////////////////////////////////////////////////////////////////////
//	Класс управления системой тактирования
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_H
#define CLOCK_SYSTEM_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\ClockSystem.h"
#include "msp430f2618\ClockSystem.h"
#include "msp430f5340\ClockSystem.h"
#include "msp430f5342\ClockSystem.h"
#include "msp430f5435a\ClockSystem.h"
#include "msp430f5438a\ClockSystem.h"
#include "msp430f5659\ClockSystem.h"
#endif

namespace Rblib
{
	// Селекстор платформа / пространство имём
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::ClockSystem,
			PlatformMsp430f2618, Msp430f2618::ClockSystem,
			PlatformMsp430f5340, Msp430f5340::ClockSystem,
			PlatformMsp430f5342, Msp430f5342::ClockSystem,
			PlatformMsp430f5435a, Msp430f5435a::ClockSystem,
			PlatformMsp430f5438a, Msp430f5438a::ClockSystem,
			PlatformMsp430f5659, Msp430f5659::ClockSystem
#endif
		>::Result ClockSystemTypes;

	// Селектор класса управления системой тактирования
	template <Platform p>
	class ClockSystemSelector: public PlatformSelector<ClockSystemTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		ClockSystemSelector()
		{
		
		}
	};

	// Класс управления системой тактирования
	typedef ClockSystemSelector<RBLIB_PALTFORM_AUTODETECT> ClockSystem;

	/// \page ClockSystem ClockSystem - Система тактирования.
	/// Система тактирования предназначена для управления различными источниками такового сигнала, настройки их частот.
	/// Эта функциональность реализована в класcе ClockSystem.
	/// \class Rblib::ClockSystem
	/// \brief Класс управления системой тактирования.
	/// \details В зависимости от выбранной платформы наследует класс системы тактирования для одного из процессоров:\n
	/// \ref Rblib::Msp430x2xx::ClockSystemParts::ClockSystem "Msp430f2618" \n
	/// \ref Msp430f5xxx::ClockSystemParts::ClockSystem "Msp430f5435a" \n
	/// \ref Msp430f5xxx::ClockSystemParts::ClockSystem "Msp430f5438a" \n

}

#endif // CLOCK_SYSTEM_H