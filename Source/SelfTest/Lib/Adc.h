///////////////////////////////////////////////////////////////////////////////
//	Класс управления модулем АЦП
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_H
#define ADC_H

#include "Platform.h"

#include "ATmega164PA\Adc.h"

namespace Lib
{
	// Селектор платформа / пространство имён
	typedef MakePlatformTypelist<
			PlatformATmega164PA, ATmega164PA::Adc
		>::Result AdcTypes;
	
	// Селектор класса для текущей платформы
	template <Platform p>
	class AdcSelector: public PlatformSelector<AdcTypes, p>::Result
	{
	private:
		// Конструктор по умолчанию (закрыт, т.к. класс полностью статический)
		AdcSelector()
		{
		
		}
	};

	// Класс для текущей платформы
	typedef AdcSelector<RBLIB_PALTFORM_AUTODETECT> Adc;
}

#endif