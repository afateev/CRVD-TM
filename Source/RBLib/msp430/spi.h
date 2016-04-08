///////////////////////////////////////////////////////////////////////////////
//	Классы SPI
//	MSP430
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_MSP430_H
#define SPI_MSP430_H

#include "../IdObjectBase.h"

namespace Rblib
{
namespace Msp430
{
	// Тип значения регистра
	typedef unsigned char SpiRegisterDataType;
	// Тип значения регистра адреса
	typedef unsigned short SpiAddressRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int  SpiRegisterAddressType;
	
	////////////////////////////////////////////////////
	// Общий для всех семейств класс SPI
	////////////////////////////////////////////////////

	template<
		unsigned int Id,
		class Hardware
		>
	class Spi :
		public IdObjectBase<Id>,
		public Hardware
	{
	public:
		// Инициализация
		static void Init(typename Hardware::ClockSource clockSource, unsigned int prescaller, bool msbFirst, bool clockPolarity, bool clockPhase)
		{
			// Инициализируем аппаратную часть
			Hardware::Init(clockSource, prescaller, msbFirst, clockPolarity, clockPhase);
		}
	};

	///////////////////////////////////////////////////////////
	// Шаблон аппаратной части SPI общий для всех семейств
	//////////////////////////////////////////////////////////
	template<
		unsigned char HardwareId,
		SpiRegisterAddressType Control0RegisterAddr,
		SpiRegisterAddressType Control1RegisterAddr,
		SpiRegisterAddressType BaudRateControl0RegisterAddr,
		SpiRegisterAddressType BaudRateControl1RegisterAddr,
		class StatusRegister,
		SpiRegisterAddressType RxBufferRegisterAddr,
		SpiRegisterAddressType TxBufferRegisterAddr,
		class InterruptControl,
		class MOSIPort, unsigned char MOSIPin,
		class MISOPort, unsigned char MISOPin,
		class CLKPort, unsigned char CLKPin
		>
	class SpiHardware :
		public UsciHardwareSpi<HardwareId,
			UsciControl< SpiRegisterDataType, SpiRegisterAddressType,
				// адреса
				Control0RegisterAddr, Control1RegisterAddr,
				// структуры
				UsciControlRegister0Spi<SpiRegisterDataType>, UsciControlRegister1Spi<SpiRegisterDataType>
			>,
			UsciBaudRateControl< SpiRegisterDataType, SpiRegisterAddressType, BaudRateControl0RegisterAddr, BaudRateControl1RegisterAddr>,
			StatusRegister,
			UsciBuffers<SpiRegisterDataType, SpiRegisterAddressType, RxBufferRegisterAddr, TxBufferRegisterAddr>,
			InterruptControl,
			GpioPeripheralFunctionControl<MOSIPort, MOSIPin, MISOPort, MISOPin, CLKPort, CLKPin>
		>
	{
	
	};
}
}

#endif