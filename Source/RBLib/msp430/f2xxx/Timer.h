///////////////////////////////////////////////////////////////////////////////
//	Класс управления аппаратными таймерами
//	MSP430F2XXX
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_MSP430F2XXX_H
#define TIMER_MSP430F2XXX_H

#include "../x1xxx,x2xxx/Timer.h"

namespace Rblib
{
namespace Msp430f2xxx
{
	// Платформозависимый набор таймеров
	class Timer
	{
	public:

		typedef	Msp430x12xxx::TimerValueDataType RegisterValueType;

	public:
		typedef Msp430x12xxx::TimerA _A; // ТA3
		typedef Msp430x12xxx::TimerB _B; // ТB7
	};
}
}

#endif
