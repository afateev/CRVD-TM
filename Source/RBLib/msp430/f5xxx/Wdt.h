///////////////////////////////////////////////////////////////////////////////
//	Класс управления сторожевым таймером
//	MSP430F5XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef WDT_MSP430F5XXX_H
#define WDT_MSP430F5XXX_H

#include <string.h>

namespace Rblib
{
namespace Msp430f5xxx
{
	// Класс управления сторожевым таймером
	template <class RegisterDataType,		// Тип (размер) значения регистра
			class PointerType,				// Тип (размер) значения адресов регистров
			PointerType ControlRegisterAddr	// Адрес регистра управления
		>
	class WtdControlRegister
	{
	protected:
		// Адрес регистра управления сторожевым таймером
		static volatile RegisterDataType * const _wdtControlRegisterAddr;

		// Структура регистра управления сторожевым таймером
		struct Struct
		{
			unsigned short Interval		: 3;	// Интервал срабатывания
			unsigned short Clear		: 1;	// Флаг сброса таймера
			unsigned short Mode			: 1;	// Режим таймера
			unsigned short ClockSource	: 2;	// Источник тактирования
			unsigned short Hold			: 1;	// Флаг остановки таймера
			unsigned short Password		: 8;	// Пароль доступа к регистру
		};
	public:
		// Допустимые интервалы срабатывания WDT
		enum WdtIntrvals
		{
			WdtInterval2G		= 0,
			WdtInterval128M		= 1,
			WdtInterval8192k	= 2,
			WdtInterval512k		= 3,
			WdtInterval32k		= 4,
			WdtInterval8192		= 5,
			WdtInterval512		= 6,
			WdtInterval64		= 7
		};
		enum WdtClockSource
		{
			WdtClockSourceSubsystemMasterClock	= 0,
			WdtClockSourceAuxClock				= 1,
			WdtClockSourceVeryLowPowerClock		= 2,
			WdtClockSourceXClock				= 3
		};
	protected:
		// Чтение значения регистра
		static inline void GetControl(Struct *value)
		{
			 memcpy(value, (const void*)_wdtControlRegisterAddr, sizeof(RegisterDataType));
		}
		// Запись значения регистра
		static inline void SetControl(Struct *value)
		{
			// выставляем обязательный пароль на запись
			value->Password = 0x5A;
			// т.к. в регистр можно писать только целым словом
			// создаем это слово
			RegisterDataType tmp;
			// копируем туда значение структуры
			memcpy(&tmp, value, sizeof(RegisterDataType));
			// записываем в регистр
			*_wdtControlRegisterAddr = tmp;
		}
    static inline void SetControlRaw(RegisterDataType value)
    {
      *_wdtControlRegisterAddr = value;
    }
	};
	
	// Инстанцирование статических переменных
	template<class RegisterDataType, class PointerType, PointerType ControlRegisterAddr>
	volatile  RegisterDataType* const WtdControlRegister<RegisterDataType, PointerType, ControlRegisterAddr>
		::_wdtControlRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(ControlRegisterAddr);
}
}

#endif