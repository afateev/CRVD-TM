///////////////////////////////////////////////////////////////////////////////
//	Классы управления разными частями модулей АЦП
//	MSP430F5XXX
//
//	Разные наборы частей разделены по разным пространствам имён по платформам
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_PARTS_MSP430F5XXX_H
#define ADC_PARTS_MSP430F5XXX_H

#include "../AdcParts.h"

namespace Rblib
{
// Части АЦП специфичные для платформы MSP430 семейства f5xxx
namespace Msp430f5xxx
{
	// Структура регистра управления 2
	struct AdcControl2Register
	{
		unsigned short ReferenceBurst				: 1; //
		unsigned short ReferenceOutputEnable		: 1; //
		unsigned short SamplingRate					: 1; //
		unsigned short SignedFormat					: 1; //
		unsigned short Resolution					: 2; //
		unsigned short Reserved0					: 1; //
		unsigned short TemperatureSensorDisable		: 1; //
		unsigned short Predivider					: 1; //
		unsigned short Reserved1					: 7; //
	};

	// Класс регистров управления АЦП
	template <class RegisterDataType,			// Тип (размер) значения регистра
			class PointerType,					// Тип (размер) значения адресов регистров
			PointerType Control0RegisterAddr,	// Адрес регистра 0 управления
			PointerType Control1RegisterAddr,	// Адрес регистра 1 управления
			PointerType Control2RegisterAddr,	// Адрес регистра 2 управления
			class Control0RegisterStructure,	// Структура регистра 0 управления
			class Control1RegisterStructure,	// Структура регистра 1 управления
			class Control2RegisterStructure		// Структура регистра 2 управления
		>
	class AdcControl : 
		public Msp430::AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
	{
	protected:
		// Адреса регистров управления
		static volatile Control2RegisterStructure * const _adcControl2RegisterAddr;
	public:
		// Установить предделитель тактового сигнала
		static inline void SetPreDivider(unsigned short value)
		{
			_adcControl2RegisterAddr->Predivider = value;
		}
	};

	// Инстанцирование статических переменных
	template<class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, PointerType Control2RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure, class Control2RegisterStructure>
	volatile  Control2RegisterStructure* const AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control2RegisterAddr, Control0RegisterStructure, Control1RegisterStructure, Control2RegisterStructure>
		::_adcControl2RegisterAddr = reinterpret_cast<volatile  Control2RegisterStructure*>(Control2RegisterAddr);
}
}

#endif