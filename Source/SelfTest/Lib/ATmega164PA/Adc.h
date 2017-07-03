///////////////////////////////////////////////////////////////////////////////
//	Класс управления модулем АЦП
//	ATmega164PA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_ATMEGA164PA_H
#define ADC_ATMEGA164PA_H

#include "../ATmega/Adc.h"
#include "../IdObjectBase.h"

namespace Lib
{
namespace ATmega164PA
{
	// Платформозависимый набор портов
	class Adc
	{
	public:
		typedef ATmega::Adc::Adc<1, 0x7C, 0x7A, 0x78, 0x79, 0x7B, 0x7E> Adc0;
	};
}
}

#endif