/////////////////////////////////////////////////////////////////////////////////
//
// Платформозависимый выбор таймера для использования в процедурах калибровки
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_CALIBRATION_TIMER_H
#define CLOCK_SYSTEM_CALIBRATION_TIMER_H

#include "./Timer.h"
#include "../../../NullType.h"

namespace Rblib
{
namespace Msp430f543x
{
	namespace ClockSystemParts
	{
		/// \brief Параметры для процедуры калибровки.
		/// \details Специфично для MSP430x5xx.
		/// \nШаблонный параметр данного класса не имеет смысла, 
		/// в клиентском коде следует использовать специализацию CalibrationParams.
		template<class UnusedTemplateArg>
		class _CalibrationParams
		{
		public:

			/// Таймер, который нужно использовать.
			typedef Msp430f543x::Timer::_C T;
			/// Блок захвата, позволяющий осуществлять захват по ACLK.
			typedef Msp430f543x::Timer::_C::CC6 CC;

			/// Константы.
			enum Constants
			{
				/// Максимальное значение счётчика таймера.
				MaxTimerCounter = 0xFFFF
			};

			/// допустимая относительная погрешность калибровки.
			static const double MaxRegulationError;

		public:

			/// Устанавливает ращмерность счётчика таймера равной 16 разрядам.
			static inline void NormalizeCounterLength()
			{
				Timer::_C::SetCounterLength(Timer::_C::CounterLength16Bit);
			}

		};

		template<class UnusedTemplateArg>
		const double _CalibrationParams<UnusedTemplateArg>::MaxRegulationError = 0.00365;
		/// \class Rblib::Msp430f543x::ClockSystemParts::CalibrationParams
		/// \brief Параметры для процедуры калибровки.
		/// \details Специфично для MSP430x5xx.
		/// \nЕдинственная специализация класса _CalibrationParams, в котором определена вся функциональность.
		/// \nОпределён как <tt>typedef _CalibrationParams<NullType> %CalibrationParams;</tt>
		typedef _CalibrationParams<NullType> CalibrationParams;
	}
}
}

#endif // CLOCK_SYSTEM_CALIBRATION_TIMER_H
