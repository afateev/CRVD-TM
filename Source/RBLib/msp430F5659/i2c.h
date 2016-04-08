///////////////////////////////////////////////////////////////////////////////
//	Класс i2c интерфейса 
//	Msp430f5659
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_MSP430F5659_H
#define I2C_MSP430F5659_H

#include "../msp430/i2c.h"
#include "../msp430/f5xxx/UsciParts.h"

namespace Rblib
{
namespace Msp430f5659
{	
	// Платформозависимый набор классов аппаратной части I2C
	class I2CHardware
	{
	public:
		typedef Msp430::I2CHardware<1, 0x05E0, 0x05E1, 0x05E6, 0x05E7,
			Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x05EA, Msp430f5xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >,
			0x05EC, 0x05EE, 0x05F0, 0x05F2,
			Msp430f5xxx::InterruptControl<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x05FC, 0x05FD, Msp430f5xxx::UsciInterruptEnableRegisterI2C<Msp430::I2CRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterI2C<Msp430::I2CRegisterDataType> >,
			Gpio::Port2, 1, Gpio::Port2, 2>	_A; // USCI_B0
		typedef Msp430::I2CHardware<2, 0x0620, 0x0621, 0x0626, 0x0627,
			Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x062A, Msp430f5xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >,
			0x062C, 0x062E, 0x0630, 0x0632,
			Msp430f5xxx::InterruptControl<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x063C, 0x063D, Msp430f5xxx::UsciInterruptEnableRegisterI2C<Msp430::I2CRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterI2C<Msp430::I2CRegisterDataType> >,
			Gpio::Port8, 5, Gpio::Port8, 6>	_B; // USCI_B1
		typedef Msp430::I2CHardware<3, 0x0661, 0x0660, 0x0666, 0x0667,
			Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x066A, Msp430f5xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >,
			0x066C, 0x066E, 0x0670, 0x0672,
			Msp430f5xxx::InterruptControl<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x067C, 0x067D, Msp430f5xxx::UsciInterruptEnableRegisterI2C<Msp430::I2CRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterI2C<Msp430::I2CRegisterDataType> >,
			Gpio::Port9, 5, Gpio::Port9, 6>	_C; // USCI_B2
	};

	// Платформозависимый набор I2C
	class I2C
	{
	public:
		class _A : public Msp430::I2C<1, I2CHardware::_A> {};
		class _B : public Msp430::I2C<2, I2CHardware::_B> {};
		class _C : public Msp430::I2C<3, I2CHardware::_C> {};
	};
}
}

#endif //I2C_MSP430F5659_H
