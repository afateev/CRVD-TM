
#ifndef CLOCK_SYSTEM_CONTROL_MSP430X1XXX_H
#define CLOCK_SYSTEM_CONTROL_MSP430X1XXX_H

#include "../x1xxx,x2xxx/ClockSystemControl.h"

namespace Rblib
{
namespace Msp430x1xxx
{
	namespace ClockSystemParts
	{
		/// Класс управления регистром DCOCTL 
		/// и настройками DCO из других управляющих регистров (BCSCTL1 и BCSCTL2).
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam DcoControlRegisterAddress Адрес регистра DCOCTL.
		/// \tparam ControlRegisterAddress1 Адрес регистра BCSCTL1.
		/// \tparam ControlRegisterAddress2 Адрес регистра BCSCTL2.
		template<class RegisterDataType,
				class PointerType,
				PointerType DcoControlRegisterAddress,
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
    class DcoControl : public Msp430x12xxx::ClockSystemParts::DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address>
		{
		public:

			/// Возвращает максимальное значение диапазона частоты (поля RSELx).
			static inline unsigned char MaxFrequencyRange()
			{
  			return 0x07;
			}

		};

	}
}
}

#endif // CLOCK_SYSTEM_CONTROL_MSP430X1XXX_H
