///////////////////////////////////////////////////////////////////////////////
//	Класс UART интерфейса 
//	Msp430f5438a
///////////////////////////////////////////////////////////////////////////////

#ifndef UART_MSP430F5438A_H
#define UART_MSP430F5438A_H

#include "../msp430/Uart.h"
#include "../msp430/f5xxx/UsciParts.h"
#include "../Gpio.h"

namespace Rblib
{
namespace Msp430f5438a
{
	// Платформозависимый набор классов аппаратной части UART
	class UartHardware
	{
	public:
		typedef Msp430::UartHardware<0x05C1, 0x05C0, 0x05C6, 0x05C7, 0x05C8, 0x05CA, 0x05CC, 0x05CE,
			Msp430f5xxx::InterruptControl<Msp430::UartRegisterDataType, Msp430::UartRegisterAddressType, 0x05DC, 0x05DD, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::UartRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterUart<Msp430::UartRegisterDataType> >,
			Gpio::Port3, 5, Gpio::Port3, 4>	_A;
		typedef Msp430::UartHardware<0x0601, 0x0600, 0x0606, 0x0607, 0x0608, 0x060A, 0x060C, 0x060E,
			Msp430f5xxx::InterruptControl<Msp430::UartRegisterDataType, Msp430::UartRegisterAddressType, 0x061C, 0x061D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::UartRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterUart<Msp430::UartRegisterDataType> >,
			Gpio::Port5, 7, Gpio::Port5, 6>	_B;
		typedef Msp430::UartHardware<0x0641, 0x0640, 0x0646, 0x0647, 0x0648, 0x064A, 0x064C, 0x064E,
			Msp430f5xxx::InterruptControl<Msp430::UartRegisterDataType, Msp430::UartRegisterAddressType, 0x065C, 0x065D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::UartRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterUart<Msp430::UartRegisterDataType> >,
			Gpio::Port9, 5, Gpio::Port5, 4>	_C;
		typedef Msp430::UartHardware<0x0681, 0x0680, 0x0686, 0x0687, 0x0688, 0x068A, 0x068C, 0x068E,
			Msp430f5xxx::InterruptControl<Msp430::UartRegisterDataType, Msp430::UartRegisterAddressType, 0x069C, 0x069D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::UartRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterUart<Msp430::UartRegisterDataType> >,
			Gpio::Port10, 5, Gpio::Port10, 4>	_D;
	};

	// Платформозависисмый набор портов
	class Uart
	{
	public:
		template<class BufferType = Msp430::UartFastFifo<255> >
		class _A : public Msp430::Uart<1, UartHardware::_A, Msp430::UartFifo<1, BufferType, BufferType> > {};

		template<class BufferType = Msp430::UartFastFifo<255> >
		class _B : public Msp430::Uart<2, UartHardware::_B, Msp430::UartFifo<2, BufferType, BufferType> > {};

		template<class BufferType = Msp430::UartFastFifo<255> >
		class _C : public Msp430::Uart<3, UartHardware::_C, Msp430::UartFifo<3, BufferType, BufferType> > {};

		template<class BufferType = Msp430::UartFastFifo<255> >
		class _D : public Msp430::Uart<4, UartHardware::_D, Msp430::UartFifo<4, BufferType, BufferType> > {};
	};
}
}

#endif //UART_MSP430F5438A_H