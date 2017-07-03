///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	ATmega164PA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_ATMEGA164PA_H
#define GPIO_ATMEGA164PA_H

#include "../ATmega/Gpio.h"
#include "../IdObjectBase.h"

namespace Lib
{
namespace ATmega164PA
{
	// Платформозависимый набор портов
	class Gpio
	{
	public:
		typedef ATmega::Gpio::Port<1, 0x22, 0x21, 0x20> A;
		typedef ATmega::Gpio::Port<2, 0x25, 0x24, 0x23> B;
		typedef ATmega::Gpio::Port<3, 0x28, 0x27, 0x26> C;
		typedef ATmega::Gpio::Port<4, 0x2B, 0x2A, 0x29> D;
	};
}
}

#endif