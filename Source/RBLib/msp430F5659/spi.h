///////////////////////////////////////////////////////////////////////////////
//	Класс SPI интерфейса 
//	Msp430f5659
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_MSP430F5659_H
#define SPI_MSP430F5659_H

#include "../msp430/spi.h"
#include "../msp430/f5xxx/UsciParts.h"

namespace Rblib
{
namespace Msp430f5659
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
			Gpio::Port2, 4, Gpio::Port2, 5, Gpio::Port2, 0>	_A;
		// B0
		typedef Msp430::SpiHardware<2, 0x05E1, 0x05E0, 0x05E6, 0x05E7, 
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x05EA, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x05EC, 0x05EE,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x05FC, 0x05FD, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port2, 1, Gpio::Port2, 2, Gpio::Port2, 3>	_B;
		// A1
		typedef Msp430::SpiHardware<3, 0x0601, 0x0600, 0x0606, 0x0607, 
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x060A, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x060C, 0x060E,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x061C, 0x061D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port8, 2, Gpio::Port8, 3, Gpio::Port8, 1>	_C;
		// B1
		typedef Msp430::SpiHardware<4, 0x0621, 0x0620, 0x0626, 0x0627, 
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x062A, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x062C, 0x062E,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x063C, 0x063D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port8, 5, Gpio::Port8, 6, Gpio::Port8, 4>	_D;
		// A2
		typedef Msp430::SpiHardware<5, 0x0641, 0x0640, 0x0646, 0x0647,
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x064A, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x064C, 0x064E,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x065C, 0x065D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port9, 2, Gpio::Port9, 3, Gpio::Port9, 1>	_E;
		// B2
		typedef Msp430::SpiHardware<6, 0x0660, 0x0660, 0x0666, 0x0667,
			Msp430::UsciStatus<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x066A, Msp430f5xxx::UsciStatusRegisterSpi<Msp430::SpiRegisterDataType> >,
			0x066C, 0x066E,
			Msp430f5xxx::InterruptControl<Msp430::SpiRegisterDataType, Msp430::SpiRegisterAddressType, 0x067C, 0x067D, Msp430f5xxx::UsciInterruptEnableRegisterUart<Msp430::SpiRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterSpi<Msp430::SpiRegisterDataType> >,
			Gpio::Port9, 5, Gpio::Port9, 6, Gpio::Port9, 4>	_F;

	};
	
	// Платформозависимый набор SPI
	class Spi
	{
	public:
		class _A : public Msp430::Spi<1, SpiHardware::_A> {};	// A0
		class _B : public Msp430::Spi<2, SpiHardware::_B> {};	// B0
		class _C : public Msp430::Spi<3, SpiHardware::_C> {};	// A1
		class _D : public Msp430::Spi<4, SpiHardware::_D> {};	// B1
		class _E : public Msp430::Spi<5, SpiHardware::_E> {};	// A2
		class _F : public Msp430::Spi<6, SpiHardware::_F> {};	// B2						
	};
}
}

#endif //UART_MSP430F5340_H