/////////////////////////////////////////////////////
//
// Cистема тактирования для семейства msp430f543x
//
/////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_MSP430F261X_H
#define CLOCK_SYSTEM_MSP430F261X_H

#include "../ClockSystem.h"
#include "./ClockSystemFrequencyRangeSelect.h"
#include "./ClockSystemCalibrationParams.h"

namespace Rblib
{
namespace Msp430f261x
{
	class ClockSystem : public Msp430x2xx::ClockSystemParts::ClockSystem<0x0056 // DCOCTL
																		, 0x0057 // BCSCTL1
																		, 0x0058 // BCSCTL2
																		, 0x0053 // BCSCTL3
																		, ClockSystemParts::FrequencyRangeSelect
																		, ClockSystemParts::CalibrationParams>
	{
	};

}
}

#endif // CLOCK_SYSTEM_MSP430F261X_H
