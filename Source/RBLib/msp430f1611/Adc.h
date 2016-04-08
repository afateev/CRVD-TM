///////////////////////////////////////////////////////////////////////////////
//	Класс управления АЦП
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_MSP430F1611_H
#define ADC_MSP430F1611_H

#include <string.h>

namespace Rblib
{
namespace Msp430f1611
{
	// Тип значения регистра
	typedef unsigned short AdcRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int  AdcRegisterAddresType;
	
	// Платформозависимый сторожевой таймер
	class Adc
	{
	public:
	};
}
}

#endif