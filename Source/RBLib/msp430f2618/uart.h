///////////////////////////////////////////////////////////////////////////////
//	Класс UART интерфейса 
//	Msp430f2618
///////////////////////////////////////////////////////////////////////////////

#ifndef UART_MSP430F2618_H
#define UART_MSP430F2618_H

#include "../msp430/Uart.h"
#include "../msp430/f2xxx/UsciParts.h"
#include "../Gpio.h"
#include "../SpecialFunctionRegisters.h"
 
namespace Rblib
{
namespace Msp430f2618
{
	// Платформозависимый набор классов аппаратной части UART
	class UartHardware
	{
	public:
		typedef Msp430::UartHardware<0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
			Msp430f2xxx::InterruptControlA0<SpecialFunctionRegisters>, Gpio::Port3, 5, Gpio::Port3, 4>	_A;
		typedef Msp430::UartHardware<0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
			Msp430f2xxx::InterruptControlA1<SpecialFunctionRegisters>, Gpio::Port3, 7, Gpio::Port3, 6>	_B;
	};

	// Платформозависимый набор портов
	class Uart
	{
	public:
		template<class BufferType = Msp430::UartFastFifo<255> >
		class _A : public Msp430::Uart<1, UartHardware::_A, Msp430::UartFifo<1, BufferType, BufferType> > {};

		template<class BufferType = Msp430::UartFastFifo<255> >
		class _B : public Msp430::Uart<2, UartHardware::_B, Msp430::UartFifo<2, BufferType, BufferType> > {};
	};
}
}

#endif //UART_MSP430F2618_H