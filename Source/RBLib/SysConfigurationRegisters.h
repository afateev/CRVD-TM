///////////////////////////////////////////////////////////////////////////////
//	Класс управления регистрами конфигурации системы
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSCONFIGURATIONREGISTERS_H
#define SYSCONFIGURATIONREGISTERS_H

#include "Platform.h"

#ifdef MSP430
#include "msp430f1611\SysConfigurationRegisters.h"
#include "msp430f2618\SysConfigurationRegisters.h"
#include "msp430f5340\SysConfigurationRegisters.h"
//#include "msp430f5342\SysConfigurationRegisters.h"
#include "msp430f5435a\SysConfigurationRegisters.h"
#include "msp430f5438a\SysConfigurationRegisters.h"
#include "msp430f5659\SysConfigurationRegisters.h"
#endif

namespace Rblib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
#ifdef MSP430
			PlatformMsp430f1611, Msp430f1611::SysConfigurationRegisters,
			PlatformMsp430f2618, Msp430f2618::SysConfigurationRegisters,
      PlatformMsp430f5340, Msp430f5340::SysConfigurationRegisters,
			//PlatformMsp430f5342, Msp430f5340::SysConfigurationRegisters,
			PlatformMsp430f5435a, Msp430f5435a::SysConfigurationRegisters,
			PlatformMsp430f5438a, Msp430f5438a::SysConfigurationRegisters,
			PlatformMsp430f5659, Msp430f5659::SysConfigurationRegisters
#endif
		>::Result SysConfigurationRegistersTypes;
	
	// Селектор класса управления регистрами конфигурации системы
	template <Platform p>
	class SysConfigurationRegistersSelector: public PlatformSelector<SysConfigurationRegistersTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		SysConfigurationRegistersSelector()
		{
		
		}
	};

	// Класс управления регистрами конфигурации системы
	typedef SysConfigurationRegistersSelector<RBLIB_PALTFORM_AUTODETECT> SysConfigurationRegisters;
}

#endif
