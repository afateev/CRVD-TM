///////////////////////////////////////////////////////////////////////////////
//	Класс i2c интерфейса 
//	Msp430f5342
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_MSP430F5342_H
#define I2C_MSP430F5342_H

#include "../msp430/i2c.h"
#include "../msp430/f5xxx/UsciParts.h"

namespace Rblib
{
namespace Msp430f5342
{	
	// Платформозависимый набор классов аппаратной части I2C
	class I2CHardware
	{
	public:
		typedef Msp430::I2CHardware<1, 0x05E1, 0x05E0, 0x05E6, 0x05E7,
			Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x05EA, Msp430f5xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >,
			0x05EC, 0x05EE, 0x05F0, 0x05F2,
			Msp430f5xxx::InterruptControl<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x05FC, 0x05FD, Msp430f5xxx::UsciInterruptEnableRegisterI2C<Msp430::I2CRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterI2C<Msp430::I2CRegisterDataType> >,
			Gpio::Port3, 0, Gpio::Port3, 1>	_A;
		typedef Msp430::I2CHardware<2, 0x0621, 0x0620, 0x0626, 0x0627,
			Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x062A, Msp430f5xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >,
			0x062C, 0x062E, 0x0630, 0x0632,
			Msp430f5xxx::InterruptControl<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x063C, 0x063D, Msp430f5xxx::UsciInterruptEnableRegisterI2C<Msp430::I2CRegisterDataType>, Msp430f5xxx::UsciInterruptFlagRegisterI2C<Msp430::I2CRegisterDataType> >,
			Gpio::Port4, 1, Gpio::Port4, 2>	_B;
	};
	
	// Платформозависимый набор I2C
	class I2C
	{
	public:
		class _A : public Msp430::I2C<1, I2CHardware::_A> {};
		class _B : public Msp430::I2C<2, I2CHardware::_B> {};
	};
}
}

#endif //I2C_MSP430F5342_H