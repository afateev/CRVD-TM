///////////////////////////////////////////////////////////////////////////////
//	Класс управления регистрами конфигурации системы
//	MSP430F5XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSCONFIGURATIONREGISTERS_MSP430F5XXX_H
#define SYSCONFIGURATIONREGISTERS_MSP430F5XXX_H

namespace Rblib
{
namespace Msp430f5xxx
{
	// Регистр вектора прерываний по сбросу
	template <class RegisterDataType,		// Тип (размер) значения регистра
			class PointerType,				// Тип (размер) значения адресов регистров
			PointerType RegisterAddr		// Адрес регистра
		>
	class SysResetInterruptVector
	{
	public:
		// Список причин перезагрузки
		enum ResetReason
		{
			ResetReasonUnknown,
			ResetReasonBrownout,
			ResetReasonRstNmi,
			ResetReasonPMMSWBOR,
			ResetReasonWakeupFromLpmX5,
			ResetReasonSecurityViolation,
			ResetReasonSVSL,
			ResetReasonSVSH,
			ResetReasonSVML_OVP,
			ResetReasonSVMH_OVP,
			ResetReasonPMMSWPOR,
			ResetReasonWdtTimeOut,
			ResetReasonWdtPasswordViolation,
			ResetReasonFlashPasswordViolation,
			ResetReasonPLLUnlock,
			ResetReasonPERFPeripheralConfigurationAreaFetch,
			ResetReasonPMMPasswordViolation,
			ResetReasonReserved
		};
	protected:	
		// Адрес регистра
		static volatile RegisterDataType * const _registerAddr;

		// Последняя причина сброса
		static ResetReason _lastResetReason;
	public:
		// Получить последнюю причину сброса
		static inline ResetReason GetLastResetReason()
		{
			// сюда временно читаем содержимое регистра,
			// т.к. прочитать его можно всего один раз, потом он будет очищен
			RegisterDataType lastRegisterValue = 0;
			// т.к. причин перезагрузки может накопиться много,
			// то читаем пока причины есть и сохраняем последнюю
			do
			{
				// читаем
				lastRegisterValue = *_registerAddr;
				// если есть причина
				if (lastRegisterValue)
				{
					// конверируем значение в понятные термины
					// запоминаем его
					switch(lastRegisterValue)
					{
					case 0x02:
						_lastResetReason = ResetReasonBrownout;
					break;
					case 0x04:
						_lastResetReason = ResetReasonRstNmi;
					break;
					case 0x06:
						_lastResetReason = ResetReasonPMMSWBOR;
					break;
					case 0x08:
						_lastResetReason = ResetReasonWakeupFromLpmX5;
					break;
					case 0x0A:
						_lastResetReason = ResetReasonSecurityViolation;
					break;
					case 0x0C:
						_lastResetReason = ResetReasonSVSL;
					break;
					case 0x0E:
						_lastResetReason = ResetReasonSVSH;
					break;
					case 0x10:
						_lastResetReason = ResetReasonSVML_OVP;
					break;
					case 0x12:
						_lastResetReason = ResetReasonSVMH_OVP;
					break;
					case 0x14:
						_lastResetReason = ResetReasonPMMSWPOR;
					break;
					case 0x16:
						_lastResetReason = ResetReasonWdtTimeOut;
					break;
					case 0x18:
						_lastResetReason = ResetReasonWdtPasswordViolation;
					break;
					case 0x1A:
						_lastResetReason = ResetReasonFlashPasswordViolation;
					break;
					case 0x1C:
						_lastResetReason = ResetReasonPLLUnlock;
					break;
					case 0x1E:
						_lastResetReason = ResetReasonPERFPeripheralConfigurationAreaFetch;
					break;
					case 0x20:
						_lastResetReason = ResetReasonPMMPasswordViolation;
					break;
					default:
						_lastResetReason = ResetReasonReserved;
					break;
					}
				}
			}
			while(lastRegisterValue);
			
			// возвращаем последнюю запоменную причину
			return _lastResetReason;
		}
	};

	// Инстанцирование статических переменных
	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	volatile  RegisterDataType* const SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>
		::_registerAddr = reinterpret_cast<volatile  RegisterDataType*>(RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType RegisterAddr>
	typename SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>::ResetReason
	SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>
		::_lastResetReason = SysResetInterruptVector<RegisterDataType, PointerType, RegisterAddr>::ResetReasonUnknown;
}
}

#endif