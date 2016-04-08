#ifndef EFM32GG_REALTIMECOUNTER_H
#define EFM32GG_REALTIMECOUNTER_H

#include "../efm32/RealTimeCounter.h"

namespace Rblib
{
namespace Efm32gg
{
	class RealTimeCounter		
	{
	public:
		typedef Efm32::RealTimeCounter::RealTimeCounter<1> Rtc;
	};
}
}

#endif