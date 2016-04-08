/////////////////////////////////////////////////////////////////////////////////
//
// Платформозависимый выбор таймера для использования в процедурах калибровки
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_CALIBRATION_TIMER_MSP430F161X_H
#define CLOCK_SYSTEM_CALIBRATION_TIMER_MSP430F161X_H

#include "../Timer.h"
#include "../../../NullType.h"

namespace Rblib
{
namespace Msp430f161x
{
	namespace ClockSystemParts
	{
		template<class UnusedTemplateArg>
		class _CalibrationParams
		{
		public:

			typedef Msp430f2xxx::Timer::_B T;
			typedef Msp430f2xxx::Timer::_B::CC6 CC;

			enum Constants
			{
				MaxTimerCounter = 0xFFFF
			};

			static const double MaxRegulationError;

		public:

			static inline void NormalizeCounterLength()
			{
			}

		};

		template<class UnusedTemplateArg>
		const double _CalibrationParams<UnusedTemplateArg>::MaxRegulationError = 0.00365;
		typedef _CalibrationParams<NullType> CalibrationParams;
	}
}
}

#endif // CLOCK_SYSTEM_CALIBRATION_TIMER_MSP430F161X_H
