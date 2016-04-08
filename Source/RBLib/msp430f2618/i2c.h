///////////////////////////////////////////////////////////////////////////////
//	Класс i2c интерфейса
//	Msp430f2618
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_MSP430F2618_H
#define I2C_MSP430F2618_H

#include "../msp430/I2C.h"
#include "../msp430/f2xxx/UsciParts.h"

namespace Rblib
{
namespace Msp430f2618
{	
	// Платформозависимый набор классов аппаратной части I2C
	class I2CHardware
	{
	public:
		typedef Msp430::I2CHardware<1, 0x0068, 0x0069, 0x006A, 0x006B,
			Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x006D, Msp430f2xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >,
			0x006E, 0x006F, 0x0118, 0x11A,
			Msp430f2xxx::InterruptControlI2C<
				Msp430f2xxx::InterruptControlB0<SpecialFunctionRegisters>,
				Msp430f2xxx::UsciI2CInterruptEnable<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x006C, Msp430f2xxx::UsciI2CInterruptEnableRegister<Msp430::I2CRegisterDataType> >,
				Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x006D, Msp430f2xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >
			>,
			Gpio::Port3, 1, Gpio::Port3, 2>	_A;
		typedef Msp430::I2CHardware<2, 0x00D8, 0x00D9, 0x00DA, 0x00DB,
			Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x00DD, Msp430f2xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >,
			0x00DE, 0x00DF, 0x017C, 0x017E,
			Msp430f2xxx::InterruptControlI2C<
				Msp430f2xxx::InterruptControlB1<SpecialFunctionRegisters>,
				Msp430f2xxx::UsciI2CInterruptEnable<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x00DC, Msp430f2xxx::UsciI2CInterruptEnableRegister<Msp430::I2CRegisterDataType> >,
				Msp430::UsciStatus<Msp430::I2CRegisterDataType, Msp430::I2CRegisterAddressType, 0x00DD, Msp430f2xxx::UsciStatusRegisterI2C<Msp430::I2CRegisterDataType> >
			>,
			Gpio::Port5, 1, Gpio::Port5, 2>	_B;
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
#endif //I2C_MSP430F2618_H