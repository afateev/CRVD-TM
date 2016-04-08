///////////////////////////////////////////////////
//
// Платформозависимый выбор диапазона частот DCO
//
///////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_FREQUENCY_RANGE_SELECT_MSP430F665X_645X_565X_535X_H
#define CLOCK_SYSTEM_FREQUENCY_RANGE_SELECT_MSP430F665X_645X_565X_535X_H

namespace Rblib
{
namespace Msp430f665x645x565x535x
{
	namespace ClockSystemParts
	{

		/// Класс, реализующий метод определения дипазона, в который попадает частота DCO.
		/// \details Специфично для MSP430x5xx.
		class FrequencyRangeSelect
		{
		public:

			/// определить в какой диапазон попадает частота.
			/// \param frequency Частота DCO в герцах.
			/// \return Диапазон от 0 до 7.
			static inline unsigned char Calculate(unsigned long frequency)
			{
				if (frequency <= 700000)
					return 0;
				else if (frequency <= 1470000)
					return 1;
				else if (frequency <= 3172000)
					return 2;
				else if (frequency <= 6000000)
					return 3;
				else if (frequency <= 12300000)
					return 4;
				else if (frequency <= 23700000)
					return 5;
				else if (frequency <= 39000000)
					return 6;
				else
					return 7;
			}

		};

	}
}
}

#endif // CLOCK_SYSTEM_FREQUENCY_RANGE_SELECTOR_MSP430F665X_645X_565X_535X_H
