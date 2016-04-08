///////////////////////////////////////////////////////////////////
//
// ������� ��������� � ���������� ������� ���������� ������������
//
///////////////////////////////////////////////////////////////////

#ifndef CLOCK_SYATEM_FREQUENCY_CALIBRATION_H
#define CLOCK_SYATEM_FREQUENCY_CALIBRATION_H

#include <math.h>
#include "../../NullType.h"

namespace Rblib
{
namespace Msp430
{
	namespace ClockSystemParts
	{

		/// ������ ��������� ������� ��������� ���������.
		/// \tparam BindControl ����� ����������� ��������� ���������� ��������� ������� � ���������� �������� ACLK, MCLK � �.�.
		/// \tparam CalibrationParams �������� ����������, ��� ��������� ��������� ��� ���������� ���������.
		/// \tparam Regulator ����� ����������� ������ ���������� � ���������� ������� ��������� ���������.
		template<class BindControl
				, class CalibrationParams
				, class Regulator = NullType>
		class FrequencyCalibration
		{
		public:

			/// ��������������� ������� ��������� ���������.
			/// \details ��� ������� "�������������" �� ������� ���������, ������� �������� ������ ��� ��������� �������.
			/// \param testClockSource ������������� ��������.
			/// \param referenceClockSource ������� ���������� ��������� �������.
			/// \param referenceSourceFreq ������� ���������� �������.
			/// \param requiredFreq �������, �� ������� ���� ��������� ��������.
			static inline unsigned long CorrectFrequency(typename BindControl::ClockSource testClockSource, // ��������, ��� ������� ����� ������������
														typename BindControl::ClockSource referenceClockSource, // �������� ��������� �������
														unsigned long referenceSourceFreq, // �������� ��������� �������
														unsigned long requiredFreq) // ��������� �������
			{
				if (0 == referenceSourceFreq || 0 == requiredFreq)
					return 0;

				double referenceClockDividerRow = (double)MinCalibrationTicks / ((double)requiredFreq / (double)referenceSourceFreq);
				typename BindControl::Divider referenceClockDivider = BindControl::GetUpDivider((unsigned char)ceil(referenceClockDividerRow));

				BindControl::SetPeripheryAuxSignalSource(referenceClockSource);
				BindControl::SetPeripheryAuxSignalDivider(referenceClockDivider);

				BindControl::SetPeripherySignalSource(testClockSource);
				BindControl::SetPeripherySignalDivider(BindControl::Divider1);
 
				// ��������� ���������� �������
				CalibrationParams::T::InterruptDisable();

				// ������ �� ��������� ������
				CalibrationParams::CC::CaptureSetMode(CalibrationParams::CC::TimerCaptureModeRising);
				// ������ ��� ������� - ACLK
				CalibrationParams::CC::CaptureSetSignal(CalibrationParams::CC::TimerCaptureInputB);
				// ��������� ����� �������
				CalibrationParams::CC::CaptureEnable();

				// �������� ������������ - SMCLK
				CalibrationParams::T::SetClockSource(CalibrationParams::T::TimerClockSourceSubsystemMasterClock);
				// ����� ��������
				CalibrationParams::T::Reset();

				unsigned char referenceDividerValue = BindControl::GetDividerValue(referenceClockDivider);
				unsigned int calibrationPeriods = 1;
				if (referenceClockDividerRow > referenceDividerValue)
				{
					calibrationPeriods = (unsigned int) ceil((double)MinCalibrationTicks / (double)(requiredFreq / (referenceSourceFreq / referenceDividerValue)));
				}
				
				unsigned int calibrationFrequency = (referenceSourceFreq / referenceDividerValue) / calibrationPeriods;
				unsigned int multiplier = requiredFreq / calibrationFrequency;

				unsigned int prevValue = 0;
				unsigned int value = 0;
				unsigned int prevValueAbsolute = 0;

				unsigned int event_counter = 0;
				unsigned int events_check = calibrationPeriods;
				double error_level = (double) multiplier * CalibrationParams::MaxRegulationError;

				CalibrationParams::CC::ClearInterruptFlag();
				// ��������� ������, continuous mode
				CalibrationParams::T::SetMode(CalibrationParams::T::TimerModeContinuous);

				unsigned int valueRelative = 0;
				
				//get walue
				while(true)
				{
					if (CalibrationParams::CC::InterruptOccured())
					{
						CalibrationParams::CC::ClearInterruptFlag();

						event_counter++;
						
						prevValue = value;
						value = CalibrationParams::CC::GetValue();
						
						if (event_counter <= _eventsSkip)
						{
							prevValueAbsolute = value;
							continue;
						}
						else if (value < prevValue)
						{
							event_counter = _eventsSkip;
							prevValueAbsolute = value;
							continue;
						}
						else if (event_counter < (events_check + _eventsSkip))
						{
							continue;
						}

						valueRelative = value - prevValueAbsolute;
	
						if (valueRelative < multiplier && (multiplier - valueRelative) > error_level)
						{
							if (!Regulator::IncrementFrequency())
								break;
						}
						else if (valueRelative > multiplier && (valueRelative - multiplier) > error_level)
						{
							if (!Regulator::DecrementFrequency())
								break;
						}
						else
						{
							break;
						}
	
						event_counter = 0;
						CalibrationParams::CC::ClearInterruptFlag();
					}
				}

				CalibrationParams::T::SetMode(CalibrationParams::T::TimerModeStop);
				return ((long) valueRelative * calibrationFrequency);
			}

			/// ���������� ������� ��������� ���������.
			/// \param testClockSource ������������� ��������.
			/// \param referenceClockSource ������� ���������� ��������� �������.
			/// \param referenceSourceFreq ������� ���������� �������.
			/// \param desiredCalibrationFreq ���������������� ��������� �������.
			static unsigned long CalcFrequency(typename BindControl::ClockSource testClockSource, // ��������, ��� ������� ����� ������
									   			typename BindControl::ClockSource referenceClockSource, // �������� ��������� �������
												unsigned long referenceSourceFreq, // �������� ��������� �������
												unsigned long desiredCalibrationFreq = 1024) // �������� ������� ���������� (��� ����, ��� ������, ����� ������)
			{
				if (0 == referenceSourceFreq || 0 == desiredCalibrationFreq)
					return 0;

				double referenceClockDividerRow = (double) referenceSourceFreq / (double) desiredCalibrationFreq;
				typename BindControl::Divider referenceClockDivider = BindControl::GetDivider((unsigned char)ceil(referenceClockDividerRow));

				BindControl::SetPeripheryAuxSignalSource(referenceClockSource);
				BindControl::SetPeripheryAuxSignalDivider(referenceClockDivider);

				BindControl::SetPeripherySignalSource(testClockSource);
				BindControl::SetPeripherySignalDivider(BindControl::Divider1);

				// ���������� ������
				CalibrationParams::T::SetMode(CalibrationParams::T::TimerModeStop);

				// ��������� ���������� �������
				CalibrationParams::T::InterruptDisable();

				// ������ �� ��������� ������
				CalibrationParams::CC::CaptureSetMode(CalibrationParams::CC::TimerCaptureModeRising);
				// ������ ��� ������� - ACLK
				CalibrationParams::CC::CaptureSetSignal(CalibrationParams::CC::TimerCaptureInputB);
				// ��������� ����� �������
				CalibrationParams::CC::CaptureEnable();

				// �������� ������������ - SMCLK
				CalibrationParams::T::SetClockSource(CalibrationParams::T::TimerClockSourceSubsystemMasterClock);
				// ����� ��������
				CalibrationParams::T::Reset();

				unsigned char referenceDividerValue = BindControl::GetDividerValue(referenceClockDivider);
				
				//unsigned int 
				double calibrationFrequency = (double) referenceSourceFreq / (double) referenceDividerValue;
				unsigned int prevValue = 0;
				unsigned int value = 0;
				unsigned int prevValueAbsolute = 0;
				unsigned long resultFrequency = 0;

				unsigned int event_counter = 0;
				unsigned int events_check = 1;
				
				// ��������� ������, continuous mode
				CalibrationParams::T::SetMode(CalibrationParams::T::TimerModeContinuous);
					
				while(true)
				{
					if (CalibrationParams::CC::InterruptOccured())
					{
						CalibrationParams::CC::ClearInterruptFlag();

						event_counter++;
						
						prevValue = value;
						value = CalibrationParams::CC::GetValue();
						
						if (event_counter <= _eventsSkip)
						{
							prevValueAbsolute = value;
							continue;
						}
						else if (value < prevValue)
						{
							event_counter = _eventsSkip;
							prevValueAbsolute = value;
							continue;
						}
						else if ((value - prevValueAbsolute) < MinCalibrationTicks)
						{
							events_check++;
							continue;
						}

						resultFrequency = (unsigned long) ((double)(value - prevValueAbsolute) * 
											((double)calibrationFrequency / events_check));
						break;
					}
				}

				// ���������� ������
				CalibrationParams::T::SetMode(CalibrationParams::T::TimerModeStop);
								
				return resultFrequency;
			}

			protected:

				static const unsigned int _eventsSkip = 2;

			public:

				static const unsigned int MinCalibrationTicks = 1000;

		};

	}
}
}
#endif // CLOCK_SYATEM_FREQUENCY_cALIBRATION_H
