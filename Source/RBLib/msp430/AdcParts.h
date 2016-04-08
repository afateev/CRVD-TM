///////////////////////////////////////////////////////////////////////////////
//	Классы управления разными частями модулей АЦП
//
//	Разные наборы частей разделены по разным пространствам имён по платформам
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_PARTS_H
#define ADC_PARTS_H

namespace Rblib
{
// Части АЦП специфичные для платформы MSP430
namespace Msp430
{
	// Структура регистра управления 0
	struct AdcControl0Register
	{
		unsigned short StartConversion				: 1; //
		unsigned short EnableConversion				: 1; //
		unsigned short ConversionTimeOverflowInterruptEnable	: 1; //
		unsigned short OverflowInterruptEnable		: 1; //
		unsigned short AdcEnable					: 1; //
		unsigned short ReferenceGeneratorEnable		: 1; //
		unsigned short ReferenceGeneratorVoltage	: 1; //
		unsigned short MultipleConversion			: 1; //
		unsigned short SampleAndHoldTime0			: 4; //
		unsigned short SampleAndHoldTime1			: 4; //
	};

	// Структура регистра управления 1
	struct AdcControl1Register
	{
		unsigned short Busy							: 1; //
		unsigned short ConversionSequenceMode		: 2; //
		unsigned short ClockSource					: 2; //
		unsigned short ClockDivider					: 3; //
		unsigned short InvertSamleAndHolsSignal		: 1; //
		unsigned short SampleAndHoldPulseMode		: 1; //
		unsigned short SampleAndHoldSource			: 2; //
		unsigned short ConversionStartAddress		: 4; //
	};

	// Структура регистра управления ячейкой преобразования
	struct AdcMemoryControlRegister
	{
		unsigned char InputChannel				: 4; // Канал
		unsigned char Reference					: 3; // Источник опорного напряжения
		unsigned char EndOfSequence				: 1; // Признак конца последовательности
	};

	// Класс регистров управления АЦП
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType Control0RegisterAddr,	// Адрес регистра 0 управления
			PointerType Control1RegisterAddr,	// Адрес регистра 1 управления
			class Control0RegisterStructure,	// Структура регистра 0 управления
			class Control1RegisterStructure		// Структура регистра 1 управления
		>
	class AdcControl
	{
	protected:
		// Адреса регистров управления
		static volatile Control0RegisterStructure * const _adcControl0RegisterAddr;
		static volatile Control1RegisterStructure * const _adcControl1RegisterAddr;
		
	public:
		// Включить модуль АЦП
		static inline void Enable()
		{
		  	_adcControl0RegisterAddr->AdcEnable = 1;
			_adcControl1RegisterAddr->SampleAndHoldPulseMode = 1;
		}
		// Выключить модуль АЦП
		static inline void Disable()
		{
			_adcControl0RegisterAddr->AdcEnable = 0;
		}
		static inline bool IsEnabled()
		{
			return _adcControl0RegisterAddr->AdcEnable == 1;
		}
		// Запустить захват одного сэмпла
		static inline void ConvertOneSample(unsigned short channel)
		{
			// Если модуль вообще не включен, то нечего делать
			// Если не проверить, то можно зависнуть в ожидании флага Busy
			if (!IsEnabled())
				return;
			// Что будем читать и откуда забирать
			_adcControl1RegisterAddr->ConversionStartAddress = channel;
			// Включаем преобразование
			_adcControl0RegisterAddr->EnableConversion = 1;
			// Начинаем захват
			_adcControl0RegisterAddr->StartConversion = 1;
			// Ждём пока завершится процесс
			while (_adcControl1RegisterAddr->Busy);
			// Выключаем преобразование
			_adcControl0RegisterAddr->EnableConversion = 0;
		}
		// Задать источник тактирования
		static inline void SetClockSource(unsigned short value)
		{
			_adcControl1RegisterAddr->ClockSource = value;
		}
		// Установить делитель тактового сигнала
		static inline void SetDivider(unsigned short value)
		{
			_adcControl1RegisterAddr->ClockDivider = value;
		}
		// Установка длительности оцифровки для каналов 0-7
		static inline void SetSampleAndHoldTimeFrom0To7(unsigned char value)
		{
			_adcControl0RegisterAddr->SampleAndHoldTime0 = value;
		}
		// Установка длительности оцифровки для каналов 8-15
		static inline void SetSampleAndHoldTimeFrom8To15(unsigned char value)
		{
			_adcControl0RegisterAddr->SampleAndHoldTime1 = value;
		}
		// Установка длительности оцифровки для всех каналов
		static inline void SetSampleAndHoldTime(unsigned char value)
		{
			SetSampleAndHoldTimeFrom0To7(value);
			SetSampleAndHoldTimeFrom8To15(value);
		}
	};

	// Инстанцирование статических переменных
	template<class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
	volatile  Control0RegisterStructure* const AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_adcControl0RegisterAddr = reinterpret_cast<volatile  Control0RegisterStructure*>(Control0RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
	volatile  Control1RegisterStructure* const AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_adcControl1RegisterAddr = reinterpret_cast<volatile  Control1RegisterStructure*>(Control1RegisterAddr);

	// Класс регистра управления ячейкой параметров и результатов преобразования
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,						// Тип (размер) значения адресов регистров
			PointerType MemoryControlRegisterAddr,	// Адрес регистра управления
			PointerType MemoryRegisterAddr,			// Адрес регистра результата
			class MemoryControlRegisterStructure	// Структура регистра управления
		>
	class AdcMemory
	{
	protected:
		// Адреса регистров управления
		static volatile MemoryControlRegisterStructure * const _memoryControlRegisterAddr;
		static volatile RegisterDataType * const _memoryRegisterAddr;
	public:
		// Установить номер канала
		static inline void SetChannel(unsigned char value)
		{
			_memoryControlRegisterAddr->InputChannel = value;
		}
		// Установить источник опорного напряжения
		static inline void SetRefSource(unsigned char value)
		{
			_memoryControlRegisterAddr->Reference = value;
		}
		// Получить результат преобразования
		static inline RegisterDataType GetResult()
		{
			return *_memoryRegisterAddr;
		}
	};
	
	// Инстанцирование статических переменных
	template<class RegisterDataType, class PointerType, PointerType MemoryControlRegisterAddr, PointerType MemoryRegisterAddr, class MemoryControlRegisterStructure>
	volatile  MemoryControlRegisterStructure* const AdcMemory<RegisterDataType, PointerType, MemoryControlRegisterAddr, MemoryRegisterAddr, MemoryControlRegisterStructure>
		::_memoryControlRegisterAddr = reinterpret_cast<volatile  MemoryControlRegisterStructure*>(MemoryControlRegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType MemoryControlRegisterAddr, PointerType MemoryRegisterAddr, class MemoryControlRegisterStructure>
	volatile  RegisterDataType* const AdcMemory<RegisterDataType, PointerType, MemoryControlRegisterAddr, MemoryRegisterAddr, MemoryControlRegisterStructure>
		::_memoryRegisterAddr = reinterpret_cast<volatile RegisterDataType*>(MemoryRegisterAddr);
	
}
}

#endif