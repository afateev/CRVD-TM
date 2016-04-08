///////////////////////////////////////////////////////////////////////////////
//	Класс управления регистрами специального назначения
//	MSP430F2XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPECIALFUNCTIONREGISTERS_MSP430F2XXX_H
#define SPECIALFUNCTIONREGISTERS_MSP430F2XXX_H

namespace Rblib
{
namespace Msp430f2xxx
{
	// Структура регистра разрешения прерываний 1
	template<class RegisterDataType>
	struct SfrInterruptEnable1Register
	{
		RegisterDataType Watchdog						: 1;	// Разрешить прерывания строжевого таймера
		RegisterDataType OscillatorFault				: 1;	// Ошибка осцилляторов
		RegisterDataType Reserved0						: 2;	// 
		RegisterDataType NmiPin							: 1;	// 
		RegisterDataType FlashControllerAccessViolation	: 1;	// 
		RegisterDataType Reserved1						: 2;	// 
	};

	// Структура регистра разрешения прерываний 2
	template<class RegisterDataType>
	struct SfrInterruptEnable2Register
	{
		RegisterDataType UCA0Rx						: 1;	// 
		RegisterDataType UCA0Tx						: 1;	// 
		RegisterDataType UCB0Rx						: 1;	// 
		RegisterDataType UCB0Tx						: 1;	//
		RegisterDataType Reserved0					: 4;	// 
	};

	// Структура регистра разрешения прерываний USCI 1
	template<class RegisterDataType>
	struct SfrUsci1InterruptEnableRegister
	{
		RegisterDataType UCA1Rx						: 1;	// 
		RegisterDataType UCA1Tx						: 1;	// 
		RegisterDataType UCB1Rx						: 1;	// 
		RegisterDataType UCB1Tx						: 1;	//
		RegisterDataType Reserved0					: 4;	// 
	};

	// Структура регистра флагов прерываний 1
	template<class RegisterDataType>
	struct SfrInterruptFlag1Register
	{
		RegisterDataType Watchdog			: 1;	// Флаг срабатывания Wdt (WDTIFG)
		RegisterDataType OscillatorFault	: 1;	// OFIFG
		RegisterDataType PowerOn			: 1;	// PORIFG
		RegisterDataType Reset				: 1;	// RSTIFG
		RegisterDataType NmiPin				: 1;	// NMIIFG
		RegisterDataType Reserved0			: 3;	// 
	};

	// Структура регистра флагов прерываний 2
	template<class RegisterDataType>
	struct SfrInterruptFlag2Register
	{
		RegisterDataType UCA0Rx						: 1;	// 
		RegisterDataType UCA0Tx						: 1;	// 
		RegisterDataType UCB0Rx						: 1;	// 
		RegisterDataType UCB0Tx						: 1;	//
		RegisterDataType Reserved0					: 4;	// 
	};

	// Структура регистра флагов прерываний USCI 1
	template<class RegisterDataType>
	struct SfrUsci1InterruptFlagRegister
	{
		RegisterDataType UCA1Rx						: 1;	// 
		RegisterDataType UCA1Tx						: 1;	// 
		RegisterDataType UCB1Rx						: 1;	// 
		RegisterDataType UCB1Tx						: 1;	//
		RegisterDataType Reserved0					: 4;	// 
	};

	// Регистр разрешения прерываний 1
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,						// Тип (размер) значения адресов регистров
			PointerType RegisterAddr				// Адрес регистра
		>
	class InterruptEnable1Register
	{
	protected:
		static volatile SfrInterruptEnable1Register<RegisterDataType> * const _registerAddr;
	public:

		static inline void EnableOscillatorFault()
		{
			_registerAddr->OscillatorFault = 1;
		}

		static inline void DisableOscillatorFault()
		{
			_registerAddr->OscillatorFault = 0;
		}

	};

	// Регистр разрешения прерываний 2
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,						// Тип (размер) значения адресов регистров
			PointerType RegisterAddr				// Адрес регистра
		>
	class InterruptEnable2Register
	{
	protected:
		static volatile SfrInterruptEnable2Register<RegisterDataType> * const _registerAddr;
	public:
		static inline void SetInterruptEnableA0Rx(bool value)
		{
			_registerAddr->UCA0Rx = value;
		}
		static inline void SetInterruptEnableA0Tx(bool value)
		{
			_registerAddr->UCA0Tx = value;
		}
		static inline void SetInterruptEnableB0Rx(bool value)
		{
			_registerAddr->UCB0Rx = value;
		}
		static inline void SetInterruptEnableB0Tx(bool value)
		{
			_registerAddr->UCB0Tx = value;
		}
	};

	// Регистр разрешения прерываний USCI 1
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,						// Тип (размер) значения адресов регистров
			PointerType RegisterAddr				// Адрес регистра
		>
	class Usci1InterruptEnableRegister
	{
	protected:
		static volatile SfrUsci1InterruptEnableRegister<RegisterDataType> * const _registerAddr;
	public:
		static inline void SetInterruptEnableA1Rx(bool value)
		{
			_registerAddr->UCA1Rx = value;
		}
		static inline void SetInterruptEnableA1Tx(bool value)
		{
			_registerAddr->UCA1Tx = value;
		}
		static inline void SetInterruptEnableB1Rx(bool value)
		{
			_registerAddr->UCB1Rx = value;
		}
		static inline void SetInterruptEnableB1Tx(bool value)
		{
			_registerAddr->UCB1Tx = value;
		}
	};

	// Регистр флагов прерываний 1
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,						// Тип (размер) значения адресов регистров
			PointerType RegisterAddr				// Адрес регистра
		>
	class InterruptFlag1Register
	{
	protected:
		static volatile SfrInterruptFlag1Register<RegisterDataType> * const _registerAddr;
	public:
		// Прочитать флаг срабатывания Watchdog
		static inline bool GetWatchdogFlag()
		{
			return _registerAddr->Watchdog;
		}
		// Очистить флаг срабатывания Watchdog
		static inline void ClearWatchdogFlag()
		{
			_registerAddr->Watchdog = 0;
		}
		// Очистить флаг ошибки осцилляторов
		static inline void ClearOscillatorFault()
		{
			_registerAddr->OscillatorFault = 0;
		}

	};

	// Регистр флагов прерываний 2
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,						// Тип (размер) значения адресов регистров
			PointerType RegisterAddr				// Адрес регистра
		>
	class InterruptFlag2Register
	{
	protected:
		static volatile SfrInterruptFlag2Register<RegisterDataType> * const _registerAddr;
	public:
		static inline bool GetInterruptA0RxFlag()
		{
			return _registerAddr->UCA0Rx;
		}
		static inline bool GetInterruptA0TxFlag()
		{
			return _registerAddr->UCA0Tx;
		}
		static inline bool GetInterruptB0RxFlag()
		{
			return _registerAddr->UCB0Rx;
		}
		static inline bool GetInterruptB0TxFlag()
		{
			return _registerAddr->UCB0Tx;
		}
		static inline void ClearInterruptA0RxFlag()
		{
			_registerAddr->UCA0Rx = 0;
		}
		static inline void ClearInterruptA0TxFlag()
		{
			_registerAddr->UCA0Tx = 0;
		}
		static inline void ClearInterruptB0RxFlag()
		{
			_registerAddr->UCB0Rx = 0;
		}
		static inline void ClearInterruptB0TxFlag()
		{
			_registerAddr->UCB0Tx = 0;
		}
	};

	// Регистр флагов прерываний USCI 1
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,						// Тип (размер) значения адресов регистров
			PointerType RegisterAddr				// Адрес регистра
		>
	class Usci1InterruptFlagRegister
	{
	protected:
		static volatile SfrUsci1InterruptFlagRegister<RegisterDataType> * const _registerAddr;
	public:
		static inline bool GetInterruptA1RxFlag()
		{
			return _registerAddr->UCA1Rx;
		}
		static inline bool GetInterruptA1TxFlag()
		{
			return _registerAddr->UCA1Tx;
		}
		static inline bool GetInterruptB1RxFlag()
		{
			return _registerAddr->UCB1Rx;
		}
		static inline bool GetInterruptB1TxFlag()
		{
			return _registerAddr->UCB1Tx;
		}
		static inline void ClearInterruptA1RxFlag()
		{
			_registerAddr->UCA1Rx = 0;
		}
		static inline void ClearInterruptA1TxFlag()
		{
			_registerAddr->UCA1Tx = 0;
		}
		static inline void ClearInterruptB1RxFlag()
		{
			_registerAddr->UCB1Rx = 0;
		}
		static inline void ClearInterruptB1TxFlag()
		{
			_registerAddr->UCB1Tx = 0;
		}
	};

	// Инстанцирование статических переменных
	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrInterruptEnable1Register<RegisterDataType>* const InterruptEnable1Register<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrInterruptEnable1Register<RegisterDataType>*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrInterruptEnable2Register<RegisterDataType>* const InterruptEnable2Register<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrInterruptEnable2Register<RegisterDataType>*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrUsci1InterruptEnableRegister<RegisterDataType>* const Usci1InterruptEnableRegister<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrUsci1InterruptEnableRegister<RegisterDataType>*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrInterruptFlag1Register<RegisterDataType>* const InterruptFlag1Register<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrInterruptFlag1Register<RegisterDataType>*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrInterruptFlag2Register<RegisterDataType>* const InterruptFlag2Register<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrInterruptFlag2Register<RegisterDataType>*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  SfrUsci1InterruptFlagRegister<RegisterDataType>* const Usci1InterruptFlagRegister<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile SfrUsci1InterruptFlagRegister<RegisterDataType>*>(RegisterAddr);
}
}

#endif