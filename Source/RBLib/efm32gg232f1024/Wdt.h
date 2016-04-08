#ifndef WDT_EFM32GG232F1024_H
#define WDT_EFM32GG232F1024_H

#include "../efm32/Wdt.h"

namespace Rblib
{
namespace Efm32gg232f1024
{
	class Wdt		
	{
	public:
		typedef Efm32::Wdt::Wdt<1, 0x40088000, 0x00, 0x04, 0x08> WDOG;
	};
}
}

#endif