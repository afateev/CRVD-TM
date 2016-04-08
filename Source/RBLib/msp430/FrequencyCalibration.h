///////////////////////////////////////////////////////////////////
//
// Функции настройки и вычисления частоты источников тактирования
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

		/// Методы настройки частоты заданного источника.
		/// \tparam BindControl Класс управляющий привязкой источников тактового сигнала к внутренним сигналам ACLK, MCLK и т.п.
		/// \tparam CalibrationParams Праметры калибровки, для настройки процедуры под конкретный процессор.
		/// \tparam Regulator Класс реализующий методы увеличения и уменьшения частоты заданного источника.
		template<class BindControl
				, class CalibrationParams
				, class Regulator = NullType>
		class FrequencyCalibration
		{
		public:

			/// Скорректировать частоту заданного источника.
			/// \details Эта функция "отталкивается" от текущих установок, поэтому работает быстро при повторных вызовах.
			/// \param testClockSource Настраиваемый источник.
			/// \param referenceClockSource Истоник эталонного тактового сигнала.
			/// \param referenceSourceFreq Частота эталонного сигнала.
			/// \param requiredFreq Частота, на которую надо настроить источник.
			static inline unsigned long CorrectFrequency(typename BindControl::ClockSource testClockSource, // источник, чью частоту будем регулировать
														typename BindControl::ClockSource referenceClockSource, // источник эталонной частоты
														unsigned long referenceSourceFreq, // значение эталонной частоты
														unsigned long requiredFreq) // требуемая частота
			{
				if (0 == referenceSourceFreq || 0 == requiredFreq)
					return 0;

				double referenceClockDividerRow = (double)MinCalibrationTicks / ((double)requiredFreq / (double)referenceSourceFreq);
				typename BindControl::Divider referenceClockDivider = BindControl::GetUpDivider((unsigned char)ceil(referenceClockDividerRow));

				BindControl::SetPeripheryAuxSignalSource(referenceClockSource);
				BindControl::SetPeripheryAuxSignalDivider(referenceClockDivider);

				BindControl::SetPeripherySignalSource(testClockSource);
				BindControl::SetPeripherySignalDivider(BindControl::Divider1);
 
				// Запрещаем прерывания таймера
				CalibrationParams::T::InterruptDisable();

				// Захват по переднему фронту
				CalibrationParams::CC::CaptureSetMode(CalibrationParams::CC::TimerCaptureModeRising);
				// Сигнал для захвата - ACLK
				CalibrationParams::CC::CaptureSetSignal(CalibrationParams::CC::TimerCaptureInputB);
				// Разрешаем режим захвата
				CalibrationParams::CC::CaptureEnable();

				// Источник тактирования - SMCLK
				CalibrationParams::T::SetClockSource(CalibrationParams::T::TimerClockSourceSubsystemMasterClock);
				// Сброс счётчика
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
				// Запускаем таймер, continuous mode
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

			/// Определить частоту заданного источника.
			/// \param testClockSource Настраиваемый источник.
			/// \param referenceClockSource Истоник эталонного тактового сигнала.
			/// \param referenceSourceFreq Частота эталонного сигнала.
			/// \param desiredCalibrationFreq Предпочтительная эталонная частота.
			static unsigned long CalcFrequency(typename BindControl::ClockSource testClockSource, // источник, чью частоту будем мерить
									   			typename BindControl::ClockSource referenceClockSource, // источник эталонной частоты
												unsigned long referenceSourceFreq, // значение эталонной частоты
												unsigned long desiredCalibrationFreq = 1024) // желаемая частота калибровки (чем ниже, тем точнее, грубо говоря)
			{
				if (0 == referenceSourceFreq || 0 == desiredCalibrationFreq)
					return 0;

				double referenceClockDividerRow = (double) referenceSourceFreq / (double) desiredCalibrationFreq;
				typename BindControl::Divider referenceClockDivider = BindControl::GetDivider((unsigned char)ceil(referenceClockDividerRow));

				BindControl::SetPeripheryAuxSignalSource(referenceClockSource);
				BindControl::SetPeripheryAuxSignalDivider(referenceClockDivider);

				BindControl::SetPeripherySignalSource(testClockSource);
				BindControl::SetPeripherySignalDivider(BindControl::Divider1);

				// остановить таймер
				CalibrationParams::T::SetMode(CalibrationParams::T::TimerModeStop);

				// Запрещаем прерывания таймера
				CalibrationParams::T::InterruptDisable();

				// Захват по переднему фронту
				CalibrationParams::CC::CaptureSetMode(CalibrationParams::CC::TimerCaptureModeRising);
				// Сигнал для захвата - ACLK
				CalibrationParams::CC::CaptureSetSignal(CalibrationParams::CC::TimerCaptureInputB);
				// Разрешаем режим захвата
				CalibrationParams::CC::CaptureEnable();

				// Источник тактирования - SMCLK
				CalibrationParams::T::SetClockSource(CalibrationParams::T::TimerClockSourceSubsystemMasterClock);
				// Сброс счётчика
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
				
				// Запускаем таймер, continuous mode
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

				// остановить таймер
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
