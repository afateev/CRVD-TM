///////////////////////////////////////////////////////////////////////////////
//	Классы управления разными частями модулей USCI
//
//	Разные наборы частей разделены по разным пространствам имён по платформам
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USCI_PARTS_MSP430_H
#define USCI_PARTS_MSP430_H

namespace Rblib
{
// Части USCI специфичные для платформы MSP430 для всех семейств
namespace Msp430
{
	//////////////////////////////////////////////////////////////
	// Регистр управления 0
	//////////////////////////////////////////////////////////////
	
	// Структура регистра управления 0
	// В режиме Uart
	template<class RegisterDataType>
	struct UsciControlRegister0Uart
	{
		RegisterDataType SynchronousModeEnable	: 1;	// Синхронный режим
		RegisterDataType Mode					: 2;	// Режим работы (Простой Uart, Мультипроцессорный, с адресным битом, с автоматическим режимом определения скорости)
		RegisterDataType StopBits				: 1;	// Количество стоповых битов
		RegisterDataType CharacterLength		: 1;	// Длина символов (7 или 8 бит)
		RegisterDataType MsbFirstEnable			: 1;	// Порядок бит
		RegisterDataType Parity					: 2;	// Четность
	};
	
	// Структура регистра управления 0
	// В режиме SPI
	template<class RegisterDataType>
	struct UsciControlRegister0Spi
	{
		RegisterDataType SynchronousModeEnable	: 1;	// Синхронный режим
		RegisterDataType Mode					: 2;	// Режим работы (3-pin SPI, 4-pin SPI, I2C)
		RegisterDataType Master					: 1;	// Master / Slave
		RegisterDataType CharacterLength		: 1;	// Длина символов (7 или 8 бит)
		RegisterDataType MsbFirstEnable			: 1;	// Порядок бит
		RegisterDataType ClockPolarity			: 1;	// Полярность тактовых импульсов
		RegisterDataType ClockPhase				: 1;	// Фаза тактовых импульсов
	};
	
	// Структура регистра управления 0
	// В режиме I2C
	template<class RegisterDataType>
	struct UsciControlRegister0I2C
	{
		RegisterDataType SynchronousModeEnable	: 1;	// Синхронный режим
		RegisterDataType Mode					: 2;	// Режим работы (3-pin SPI, 4-pin SPI, I2C)
		RegisterDataType Master					: 1;	// Master / Slave
		RegisterDataType Reserved				: 1;	// 
		RegisterDataType MultiMasterEnable		: 1;	// Режим с несколькими мастерами
		RegisterDataType SlaveAddressSize		: 1;	// Размер адреса ведомого (7 или 10 бит)
		RegisterDataType OwnAddressSize			: 1;	// Размер своего адреса (7 или 10 бит)
	};

	//////////////////////////////////////////////////////////////
	// Регистр управления 1
	//////////////////////////////////////////////////////////////

	// Структура регистра управления 1
	// В режиме Uart
	template<class RegisterDataType>
	struct UsciControlRegister1Uart
	{
		RegisterDataType Reset									: 1;	// Сброс
		RegisterDataType TransmitBreak							: 1;	// Передать Break в следующем кадре
		RegisterDataType TransmitAddress						: 1;	// Передать Адрес в следующем кадре
		RegisterDataType Dormant								: 1;	// Заснуть в терминах Uart
		RegisterDataType ReceiveBreakInterruptEnable			: 1;	// Разрешить прерывание Rx при получени Break
		RegisterDataType ReceiveErroneousCharInterruptEnable	: 1;	// Разрешить прерывание при получении битых символов
		RegisterDataType ClockSource							: 2;	// Источник тактирования
	};

	// Структура регистра управления 1
	// В режиме SPI
	template<class RegisterDataType>
	struct UsciControlRegister1Spi
	{
		RegisterDataType Reset			: 1;	// Сброс
		RegisterDataType Reserved		: 5;	// 
		RegisterDataType ClockSource	: 2;	// Источник тактирования
	};

	// Структура регистра управления 1
	// В режиме I2C
	template<class RegisterDataType>
	struct UsciControlRegister1I2C
	{
		RegisterDataType Reset				: 1;	// Сброс
		RegisterDataType TransmitStart		: 1;	// Generate START condition
		RegisterDataType TransmitStop		: 1;	// Generate STOP
		RegisterDataType TransmitNack		: 1;	// Generate NACK
		RegisterDataType TransmitterEnable	: 1;	// 1 - Передатчик, 0 - Приемник
		RegisterDataType Reserved			: 1;	// 
		RegisterDataType ClockSource		: 2;	// Источник тактирования
	};
	
	//////////////////////////////////////////////////////////////
	// Регистр управления модуляцией
	//////////////////////////////////////////////////////////////

	// Структура регистра управления модуляцией
	template<class RegisterDataType>
	struct UsciModulationControlRegister
	{
		RegisterDataType OversamplingEnable		: 1;	// 
		RegisterDataType SecondModulationStage	: 3;	// 
		RegisterDataType FirstModulationStage	: 4;	// 
	};

	//////////////////////////////////////////////////////////////
	// Регистр статуса
	//////////////////////////////////////////////////////////////

	// Структура регистра статуса
	// В режиме Uart
	template<class RegisterDataType>
	struct UsciStatusRegisterUart
	{
		RegisterDataType Busy					: 1;	// 
		RegisterDataType IdleOrAddressReceived	: 1;	// 
		RegisterDataType RxError				: 1;	// 
		RegisterDataType Break					: 1;	// 
		RegisterDataType ParityError			: 1;	// 
		RegisterDataType OverrunError			: 1;	// 
		RegisterDataType FramingError			: 1;	// 
		RegisterDataType Listen					: 1;	// 
	};

	// Структура регистра статуса
	// В режиме Spi
	template<class RegisterDataType>
	struct UsciStatusRegisterSpi
	{
		RegisterDataType Busy			: 1;	// 
		RegisterDataType Reserved		: 4;	// 
		RegisterDataType OverrunError	: 1;	// 
		RegisterDataType FramingError	: 1;	// 
		RegisterDataType Listen			: 1;	// 
	};

	//////////////////////////////////////////////////////////////
	// Управление портами GPIO
	//////////////////////////////////////////////////////////////

	// Класс управления портами Gpio, связанными с интерфейсом
	template<class port1, unsigned char pin1, class port2, unsigned char pin2, class port3 = int, unsigned char pin3 = 0>
	class GpioPeripheralFunctionControl
	{
	public:
		typedef port1 Port1;
		typedef port2 Port2;
		typedef port3 Port3;
		static const unsigned char Pin1 = pin1;
		static const unsigned char Pin2 = pin2;
		static const unsigned char Pin3 = pin3;
	public:
		static inline void EnableUartFunction()
		{
			Port1::SetFunctionPeripheral(Pin1);
			Port2::SetFunctionPeripheral(Pin2);
		}

		static inline void EnableI2CFunction()
		{
			Port1::SetFunctionPeripheral(Pin1);
			Port2::SetFunctionPeripheral(Pin2);
		}
		
		static inline void EnableSpiFunction()
		{
			Port1::SetFunctionPeripheral(Pin1);
			Port2::SetFunctionPeripheral(Pin2);
			Port3::SetFunctionPeripheral(Pin3);
		}
		
		static inline void DisableUartFunction()
		{
			Port1::SetFunctionGeneralIo(Pin1);
			Port2::SetFunctionGeneralIo(Pin2);
		}

		static inline void DisabelI2CFunction()
		{
			Port1::SetFunctionGeneralIo(Pin1);
			Port2::SetFunctionGeneralIo(Pin2);
		}
		
		static inline void DisabelSpiFunction()
		{
			Port1::SetFunctionGeneralIo(Pin1);
			Port2::SetFunctionGeneralIo(Pin2);
			Port3::SetFunctionGeneralIo(Pin3);
		}
	};
}
}

#endif