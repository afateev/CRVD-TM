
#ifndef CLOCK_SYSTEM_CLODK_SOURCES_MSP430X1XXX_H
#define CLOCK_SYSTEM_CLODK_SOURCES_MSP430X1XXX_H

#include "ClockSystemControl.h"
#include "../FrequencyCalibration.h"
#include "../x1xxx,x2xxx/ClockSystemClockSources.h"

namespace Rblib
{
namespace Msp430x1xxx
{
	namespace ClockSystemParts
	{
		/// \brief Внутренний источник DCO.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam DcoControlRegisterAddress Адрес регистра DCOCTL.
		/// \tparam ControlRegister1Address Адрес регистра BCSCTL1.
		/// \tparam ControlRegister2Address Адрес регистра BCSCTL2.
		/// \tparam ControlRegister3Address Адрес регистра BCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType DcoControlRegisterAddress,
				PointerType ControlRegister1Address,
				PointerType ControlRegister2Address,
				class FrequencyRangeSelect,
				class CalibrationParams>
    class InternalClockSourceImpl : public Msp430x12xxx::ClockSystemParts::InternalClockSourceImpl<RegisterDataType,
                                                                                                    PointerType,
                                                                                                    ControlRegister1Address,
                                                                                                    ControlRegister2Address,
                                                                                                    DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address>,
                                                                                                    Msp430x12xxx::ClockSystemParts::ClockSourceBindControl<RegisterDataType, PointerType, ControlRegister1Address, ControlRegister2Address>,
                                                                                                    FrequencyRangeSelect,
                                                                                                    CalibrationParams>
		{
		public:

		};

	}
}
}

#endif // CLOCK_SYSTEM_CLODK_SOURCES_MSP430X1XXX_H
