///////////////////////////////////////////////////////////////////////////////
//	Класс SPI интерфейса 
//	Msp430f5340
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_MSP430F5340_H
#define SPI_MSP430F5340_H

#include "../msp430/spi.h"
#include "../msp430/f5xxx/UsciParts.h"

namespace Rblib
{
namespace Msp430f5340
{
	// Платформозависимый набор классов аппаратной части SPI
	class SpiHardware
	{
	public:
		// A0
		typedef Msp430::SpiHardware<1, 0x05C1, 0x05C0, 0x05C6, 0x05C7, 
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x05CA, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x05CC, 0x05CE,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x05DC, 0x05DD, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port3, 3, Gpio::Port3, 4, Gpio::Port2, 7>	_A;
		// B0
		typedef Msp430::SpiHardware<2, 0x05E1, 0x05E0, 0x05E6, 0x05E7, 
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x05EA, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x05EC, 0x05EE,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x05FC, 0x05FD, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port3, 0, Gpio::Port3, 1, Gpio::Port3, 2>	_B;
		// A1
		typedef Msp430::SpiHardware<3, 0x0601, 0x0600, 0x0606, 0x0607, 
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x060A, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x060C, 0x060E,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x061C, 0x061D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port4, 4, Gpio::Port4, 5, Gpio::Port4, 0>	_C;
		// B1
		typedef Msp430::SpiHardware<4, 0x0621, 0x0620, 0x0626, 0x0627, 
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x062A, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x062C, 0x062E,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x063C, 0x063D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port4, 1, Gpio::Port4, 2, Gpio::Port4, 3>	_D;
	};
	
	// Платформозависимый набор SPI
	class Spi
	{
	public:
		class _A : public Msp430::Spi<1, SpiHardware::_A> {};	// A0
		class _B : public Msp430::Spi<2, SpiHardware::_B> {};	// B0
		class _C : public Msp430::Spi<3, SpiHardware::_C> {};	// A1
		class _D : public Msp430::Spi<4, SpiHardware::_D> {};	// B1
	};
}
}

#endif //UART_MSP430F5340_H