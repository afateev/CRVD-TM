/////////////////////////////////////////////////////
//
// Cистема тактирования для семейства msp430f543x
//
/////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_MSP430F665X_645X_565X_535X_H
#define CLOCK_SYSTEM_MSP430F665X_645X_565X_535X_H

#include "../ClockSystem.h"
#include "./ClockSystemFrequencyRangeSelect.h"
#include "./ClockSystemCalibrationParams.h"

namespace Rblib
{
namespace Msp430f665x645x565x535x
{
	class ClockSystem : public Msp430f5xxx::ClockSystemParts::ClockSystem<0x0160 // UCSCTL0
																		, 0x0162 // UCSCTL1
																		, 0x0164 // UCSCTL2
																		, 0x0166 // UCSCTL3
																		, 0x0168 // UCSCTL4
																		, 0x016A // UCSCTL5
																		, 0x016C // UCSCTL6
																		, 0x016E // UCSCTL7
																		, ClockSystemParts::FrequencyRangeSelect
																		, ClockSystemParts::CalibrationParams>
	{
	};

}
}

#endif // CLOCK_SYSTEM_MSP430F665X_645X_565X_535X_H
