
#ifndef POWER_MANAGMENT_MODULE_MSP430F543X_H
#define POWER_MANAGMENT_MODULE_MSP430F543X_H

#include "../PowerManagmentModule.h"

namespace Rblib
{
	namespace Msp430f543x
	{
		typedef Msp430f5xxx::PowerManagmentModule<unsigned int, 0x0120> PowerManagmentModule;
	}
}

#endif // POWER_MANAGMENT_MODULE_MSP430F543X_H
