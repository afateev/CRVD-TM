///////////////////////////////////////////////////
//
// ѕлатформозависимый выбор диапазона частот DCO
//
///////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_FREQUENCY_RANGE_SELECT_MSP430F161X_H
#define CLOCK_SYSTEM_FREQUENCY_RANGE_SELECT_MSP430F161X_H

namespace Rblib
{
namespace Msp430f161x
{
	namespace ClockSystemParts
	{

		class FrequencyRangeSelect
		{
		public:

			// определить в какой диапазон попадает частота
			static inline unsigned char Calculate(unsigned long frequency)
			{
				if (frequency <= 100000)
					return 0;
				else if (frequency <= 140000)
					return 1;
				else if (frequency <= 200000)
					return 2;
				else if (frequency <= 280000)
					return 3;
				else if (frequency <= 390000)
					return 4;
				else if (frequency <= 540000)
					return 5;
				else if (frequency <= 800000)
					return 6;
				else if (frequency <= 1100000)
					return 7;
				else if (frequency <= 1600000)
					return 8;
				else if (frequency <= 2500000)
					return 9;
				else if (frequency <= 3000000)
					return 10;
				else if (frequency <= 4300000)
					return 11;
				else if (frequency <= 6000000)
					return 12;
				else if (frequency <= 8600000)
					return 13;
				else if (frequency <= 1200000)
					return 14;
				else
					return 15;
			}

		};

	}
}
}

#endif // CLOCK_SYSTEM_FREQUENCY_RANGE_SELECT_MSP430F161X_H
