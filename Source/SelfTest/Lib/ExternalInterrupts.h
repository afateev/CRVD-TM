///////////////////////////////////////////////////////////////////////////////
//	Класс управления внешними прерываниями
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef EXTERNALINTERRUPTS_H
#define EXTERNALINTERRUPTS_H

#include "Platform.h"

#include "ATmega164PA\ExternalInterrupts.h"

namespace Lib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::ExternalInterrupts
		>::Result ExternalInterruptsTypes;
	
	// Селектор класса для текущей платформы
	template <Platform p>
	class ExternalInterruptsSelector: public PlatformSelector<ExternalInterruptsTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		ExternalInterruptsSelector()
		{
		
		}
	};

	// Класс для текущей платформы
	typedef ExternalInterruptsSelector<RBLIB_PALTFORM_AUTODETECT> ExternalInterrupts;
}

#endif