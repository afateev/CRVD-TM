///////////////////////////////////////////////////////////////////////////////
//	����� UART ���������� 
//	Msp430f5340
///////////////////////////////////////////////////////////////////////////////

#ifndef UART_MSP430F5342_H
#define UART_MSP430F5342_H

#include "../msp430/Uart.h"
#include "../msp430/f5xxx/UsciParts.h"
#include "../Gpio.h"

namespace Rblib
{
namespace Msp430f5342
{
	// ������������������ ����� ������� ���������� ����� UART
	class UartHardware
	{
	public:
		typedef Msp430::UartHardware<0x05C1, 0x05C0, 0x05C6, 0x05C7, 0x05C8, 0x05CA, 0x05CC, 0x05CE,
			Msp430f5xxx::InterruptControl<Msp430::UartRegisterDataType, Msp430::UartRegisterAddressType, 0x05DC, 0x05DD, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::UartRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterUart<Msp430::UartRegisterDataType> >,
			Gpio::Port3, 4, Gpio::Port3, 3>	_A;
		typedef Msp430::UartHardware<0x0601, 0x0600, 0x0606, 0x0607, 0x0608, 0x060A, 0x060C, 0x060E,
			Msp430f5xxx::InterruptControl<Msp430::UartRegisterDataType, Msp430::UartRegisterAddressType, 0x061C, 0x061D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::UartRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterUart<Msp430::UartRegisterDataType> >,
			Gpio::Port4, 5, Gpio::Port4, 4>	_B;
	};

	// ������������������� ����� ������
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

#endif //UART_MSP430F5342_H