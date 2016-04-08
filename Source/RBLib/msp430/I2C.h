///////////////////////////////////////////////////////////////////////////////
//	Классы I2C
//	MSP430
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_MSP430_H
#define I2C_MSP430_H

#include "../IdObjectBase.h"

namespace Rblib
{
namespace Msp430
{
	// Тип значения регистра
	typedef unsigned char I2CRegisterDataType;
	// Тип значения регистра адреса
	typedef unsigned short I2CAddressRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int  I2CRegisterAddressType;
	
	////////////////////////////////////////////////////
	// Общий для всех семейств класс I2C
	////////////////////////////////////////////////////

	template<
		unsigned int Id,
		class Hardware
		>
	class I2C :
		public IdObjectBase<Id>,
		public Hardware
	{
	public:
		// Инициализация
		static void Init(typename Hardware::ClockSource clockSource, unsigned int prescaller)
		{
			// Инициализируем аппаратную часть
			Hardware::Init(clockSource, prescaller);
		}
	};

	///////////////////////////////////////////////////////////
	// Шаблон аппаратной части I2C общий для всех семейств
	//////////////////////////////////////////////////////////
	template<
		unsigned char HardwareId,
		I2CRegisterAddressType Control0RegisterAddr,
		I2CRegisterAddressType Control1RegisterAddr,
		I2CRegisterAddressType BaudRateControl0RegisterAddr,
		I2CRegisterAddressType BaudRateControl1RegisterAddr,
		class StatusRegister,
		I2CRegisterAddressType RxBufferRegisterAddr,
		I2CRegisterAddressType TxBufferRegisterAddr,
		I2CRegisterAddressType OwnAddressRegisterAddr,
		I2CRegisterAddressType SlaveAddressRegisterAddr,
		class InterruptControl,
		class SDAPort, unsigned char SDAPin,
		class SCLPort, unsigned char SCLPin
		>
	class I2CHardware :
		public UsciHardwareI2C<HardwareId,
			UsciControl< I2CRegisterDataType, I2CRegisterAddressType,
				// адреса
				Control0RegisterAddr, Control1RegisterAddr,
				// структуры
				UsciControlRegister0I2C<I2CRegisterDataType>, UsciControlRegister1I2C<I2CRegisterDataType>
			>,
			UsciBaudRateControl< I2CRegisterDataType, I2CRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>,
			StatusRegister,
			UsciBuffers<I2CRegisterDataType, I2CRegisterAddressType, RxBufferRegisterAddr, TxBufferRegisterAddr>,
			UsciI2CAddress<I2CAddressRegisterDataType, I2CRegisterAddressType, OwnAddressRegisterAddr, SlaveAddressRegisterAddr>,
			InterruptControl,
			GpioPeripheralFunctionControl<SDAPort, SDAPin, SCLPort, SCLPin>
		>
	{
	
	};
}
}

#endif