///////////////////////////////////////////////////////////////////////////////
//	Класс управления аппаратными таймерами
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_MSP430F1611_H
#define TIMER_MSP430F1611_H

#include "../msp430/x1xxx,x2xxx/Timer.h"

namespace Rblib
{
namespace Msp430f1611
{
	// Платформозависимый набор таймеров
	class Timer
	{
	public:

    typedef Msp430x12xxx::TimerValueDataType RegisterValueType;

  public:

    typedef Msp430x12xxx::TimerA _A; // ТA3
    typedef Msp430x12xxx::TimerB _B; // ТB7

	};
}
}

#endif
