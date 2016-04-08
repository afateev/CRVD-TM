/////////////////////////////////////////////////////////////////////////////////
//
// ������������������ ����� ������� ��� ������������� � ���������� ����������
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_CALIBRATION_TIMER_MSP430F665X_645X_565X_535X_H
#define CLOCK_SYSTEM_CALIBRATION_TIMER_MSP430F665X_645X_565X_535X_H

#include "./Timer.h"
#include "../../../NullType.h"

namespace Rblib
{
namespace Msp430f665x645x565x535x
{
	namespace ClockSystemParts
	{
		/// \brief ��������� ��� ��������� ����������.
		/// \details ���������� ��� Msp430f665x, 645x, 565x, 535x.
		/// \n��������� �������� ������� ������ �� ����� ������, 
		/// � ���������� ���� ������� ������������ ������������� CalibrationParams.
		template<class UnusedTemplateArg>
		class _CalibrationParams
		{
		public:

			/// ������, ������� ����� ������������.
			typedef Msp430f543x::Timer::_B T;
			/// ���� �������, ����������� ������������ ������ �� ACLK.
			typedef Msp430f543x::Timer::_B::CC2 CC;

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
				// �� �������������� �������� �1. �� ��������� 16 ���.
			}

		};

		template<class UnusedTemplateArg>
		const double _CalibrationParams<UnusedTemplateArg>::MaxRegulationError = 0.00365;
		/// \class Rblib::Msp430f543x::ClockSystemParts::CalibrationParams
		/// \brief ��������� ��� ��������� ����������.
		/// \details ���������� ��� Msp430f665x, 645x, 565x, 535x.
		/// \n������������ ������������� ������ _CalibrationParams, � ������� ���������� ��� ����������������.
		/// \n�������� ��� <tt>typedef _CalibrationParams<NullType> %CalibrationParams;</tt>
		typedef _CalibrationParams<NullType> CalibrationParams;
	}
}
}

#endif // CLOCK_SYSTEM_CALIBRATION_TIMER_MSP430F665X_645X_565X_535X_H
