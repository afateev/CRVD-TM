#ifndef ADC_EFM32GG232F1024_H
#define ADC_EFM32GG232F1024_H

#include "../efm32/Adc.h"

namespace Rblib
{
namespace Efm32gg232f1024
{
	class Adc		
	{
	public:
		typedef Efm32::Adc::Adc<1, 0x40002000, 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24,  0x28, 0x2C, 0x30, 0x34, 0x3C> Adc0;
	};
}
}

#endif