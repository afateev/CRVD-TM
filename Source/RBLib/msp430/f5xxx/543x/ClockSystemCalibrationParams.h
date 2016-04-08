/////////////////////////////////////////////////////////////////////////////////
//
// ������������������ ����� ������� ��� ������������� � ���������� ����������
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
		/// \brief ��������� ��� ��������� ����������.
		/// \details ���������� ��� MSP430x5xx.
		/// \n��������� �������� ������� ������ �� ����� ������, 
		/// � ���������� ���� ������� ������������ ������������� CalibrationParams.
		template<class UnusedTemplateArg>
		class _CalibrationParams
		{
		public:

			/// ������, ������� ����� ������������.
			typedef Msp430f543x::Timer::_C T;
			/// ���� �������, ����������� ������������ ������ �� ACLK.
			typedef Msp430f543x::Timer::_C::CC6 CC;

			/// ���������.
			enum Constants
			{
				/// ������������ �������� �������� �������.
				MaxTimerCounter = 0xFFFF
			};

			/// ���������� ������������� ����������� ����������.
			static const double MaxRegulationError;

		public:

			/// ������������� ����������� �������� ������� ������ 16 ��������.
			static inline void NormalizeCounterLength()
			{
				Timer::_C::SetCounterLength(Timer::_C::CounterLength16Bit);
			}

		};

		template<class UnusedTemplateArg>
		const double _CalibrationParams<UnusedTemplateArg>::MaxRegulationError = 0.00365;
		/// \class Rblib::Msp430f543x::ClockSystemParts::CalibrationParams
		/// \brief ��������� ��� ��������� ����������.
		/// \details ���������� ��� MSP430x5xx.
		/// \n������������ ������������� ������ _CalibrationParams, � ������� ���������� ��� ����������������.
		/// \n�������� ��� <tt>typedef _CalibrationParams<NullType> %CalibrationParams;</tt>
		typedef _CalibrationParams<NullType> CalibrationParams;
	}
}
}

#endif // CLOCK_SYSTEM_CALIBRATION_TIMER_H
