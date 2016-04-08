//////////////////////////////
//
// Классы тактовых сигналов
//
//////////////////////////////

#ifndef CLOCK_SYSTEM_CLOCK_SIGNALS_MSP430X2XX
#define CLOCK_SYSTEM_CLOCK_SIGNALS_MSP430X2XX

#include "../x1xxx,x2xxx/ClockSystemClockSignals.h"

namespace Rblib
{
namespace Msp430x2xx
{
	namespace ClockSystemParts
	{
		//
		// MCLK
		//
    class CoreClockSignalImpl : public Msp430x12xxx::ClockSystemParts::CoreClockSignalImpl
		{
		};

		//
		// SMCLK
		//
		class PeripheryClockSignalImpl : public Msp430x12xxx::ClockSystemParts::PeripheryClockSignalImpl
		{
		};

		//
		// ACLK
		//
		class PeripheryAuxClockSignalImpl : public Msp430x12xxx::ClockSystemParts::PeripheryAuxClockSignalImpl
		{
		};

	}
}
}

#endif // CLOCK_SYSTEM_CLOCK_SIGNALS_MSP430X2XX
