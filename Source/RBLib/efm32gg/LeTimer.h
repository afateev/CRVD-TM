#ifndef EFM32GG_LETIMER_H
#define EFM32GG_LETIMER_H

#include "../efm32/LeTimer.h"

namespace Rblib
{
namespace Efm32gg
{
	class LeTimer		
	{
	public:
		typedef Efm32::LeTimer::LeTimer<1, 0x40082000, 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C,
			0x20, 0x24, 0x28, 0x2C,	0x30, 0x34, 0x40> LETIMER0;
	};
}
}

#endif