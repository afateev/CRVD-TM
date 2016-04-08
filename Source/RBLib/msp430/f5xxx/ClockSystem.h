///////////////////////////////////////////////////////////////////
//
// Cистема тактирования для семейства msp430x5xxx / msp430x6xxx
//
///////////////////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_MSP430F5XXX_H
#define CLOCK_SYSTEM_MSP430F5XXX_H

#include "../../Common.h"
#include "../../OperatingMode.h"

#include "./ClockSystemClockSources.h"
#include "./ClockSystemClockSignals.h"
#include "../FrequencyCalibration.h"
#include "../../SpecialFunctionRegisters.h"

namespace Rblib
{
namespace Msp430f5xxx
{
	namespace ClockSystemParts
	{
		/// Тип значения регистров системы тактирования
		typedef unsigned short RegisterValueType;
		/// Тип адреса регистра системы тактирования
		typedef unsigned int RegisterAddresType;

		/// \brief Класс системы тактирования.
		/// \tparam DcoRegister1Address Адрес регистра UCSCTL0
		/// \tparam DcoRegister2Address Адрес регистра UCSCTL1
		/// \tparam FllRegister1Address Адрес регистра 
		/// \tparam FllRegister2Address Адрес регистра UCSCTL3
		/// \tparam BindControlRegister1Address Адрес регистра UCSCTL4
		/// \tparam BindControlRegister2Address Адрес регистра UCSCTL5
		/// \tparam GeneralSettingsRegisterAddress Адрес регистра UCSCTL6
		/// \tparam FaultFlagsRegisterAddress Адрес регистра UCSCTL7
		/// \tparam FrequencyRangeSelect Платформозависимый класс, определяющий в какой диапазон попадает частота.
		/// \tparam CalibrationParams Платформозависимые праметры дял процедуры калибровки
		template<RegisterAddresType DcoRegister1Address, // UCSCTL0
				RegisterAddresType DcoRegister2Address, // UCSCTL1
				RegisterAddresType FllRegister1Address, // UCSCTL2
				RegisterAddresType FllRegister2Address, // UCSCTL3
				RegisterAddresType BindControlRegister1Address, // UCSCTL4
				RegisterAddresType BindControlRegister2Address, // UCSCTL5
				RegisterAddresType GeneralSettingsRegisterAddress, // UCSCTL6
				RegisterAddresType FaultFlagsRegisterAddress, // UCSCTL7
				class FrequencyRangeSelect,
				class CalibrationParams>
		class ClockSystem
		{
		public:

			/// Связь источник-сигнал, управление делителями.
			typedef ClockSourceBindControl<RegisterValueType, RegisterAddresType
										, BindControlRegister1Address, BindControlRegister2Address> BindControl;

			/// Доступ к флагам ошибок генераторов.
			typedef ClockSourceFaultFlagsControl<RegisterValueType, RegisterAddresType
											, FaultFlagsRegisterAddress> FaultFlagsControl;			

			//
			// источники
			//

			/// Внешний источник XT1.
			typedef ExternalClockSource1Impl<RegisterValueType, RegisterAddresType
											, GeneralSettingsRegisterAddress> ExternalClockSource1;

			/// Внешний источник XT2.
			typedef ExternalClockSource2Impl<RegisterValueType, RegisterAddresType
											, GeneralSettingsRegisterAddress> ExternalClockSource2;

			/// Внутренний источник DCO.
			typedef InternalClockSourceImpl<RegisterValueType, RegisterAddresType
											, DcoRegister1Address, DcoRegister2Address
											, FllRegister1Address, FllRegister2Address
											, BindControlRegister1Address, BindControlRegister2Address
											, FrequencyRangeSelect
											, CalibrationParams> InternalClockSource;

			/// Внутренний источник DCODIV.
			typedef InternalDivClockSourceImpl<RegisterValueType, RegisterAddresType
											, FllRegister1Address, FllRegister2Address> InternalDivClockSource;

			/// Внутренний источник REFOCLK.
			typedef InternalLowFreqClockSource1Impl InternalLowFreqClockSource1;

			/// Внутренний источник VLOCLK.
			typedef InternalLowFreqClockSource2Impl InternalLowFreqClockSource2;

			//
			// Сигналы
			//

			/// Тактовый сигнал MCLK.
			struct CoreClockSignal : public CoreClockSignalImpl
			{
				/// Задать источник, установить делитель равным 1.
				/// \param source Источник тактового сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					BindControl::SetCoreSignalSource(source);
					BindControl::SetCoreSignalDivider(BindControl::Divider1);
					return true;
				}

				/// Задать источник и делитель.
				/// \param source Источник тактового сигнала.
				/// \param divider Делитель.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					BindControl::SetCoreSignalSource(source);
					BindControl::SetCoreSignalDivider(divider);
					return true;
				}

				/// Задать источник и подобрать делитель.
				/// \details Делитель будет выбран на основе указанной частоты источника и желаемой частоты сигнала.
				/// \param source Источник тактового сигнала.
				/// \param sourceFreq Частота источника.
				/// \param signalFreq Желаемая результирующая часто сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source, unsigned long sourceFreq, unsigned long signalFreq)
				{
					BindControl::SetCoreSignalSource(source);
					BindControl::SetCoreSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// Тактовый сигнал SMCLK.
			struct PeripheryClockSignal : public PeripheryClockSignalImpl
			{
				/// Задать источник, установить делитель равным 1.
				/// \param source Источник тактового сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					BindControl::SetPeripherySignalSource(source);
					BindControl::SetPeripherySignalDivider(BindControl::Divider1);
					return true;
				}

				/// Задать источник и делитель.
				/// \param source Источник тактового сигнала.
				/// \param divider Делитель.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					BindControl::SetPeripherySignalSource(source);
					BindControl::SetPeripherySignalDivider(divider);
					return true;
				}

				/// Задать источник и подобрать делитель.
				/// \details Делитель будет выбран на основе указанной частоты источника и желаемой частоты сигнала.
				/// \param source Источник тактового сигнала.
				/// \param sourceFreq Частота источника.
				/// \param signalFreq Желаемая результирующая часто сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source, unsigned long sourceFreq, unsigned long signalFreq)
				{
					BindControl::SetPeripherySignalSource(source);
					BindControl::SetPeripherySignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// Тактовый сигнал ACLK.
			struct PeripheryAuxClockSignal : public PeripheryAuxClockSignalImpl
			{
				/// Задать источник, установить делитель равным 1.
				/// \param source Источник тактового сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					BindControl::SetPeripheryAuxSignalSource(source);
					BindControl::SetPeripheryAuxSignalDivider(BindControl::Divider1);
					return true;
				}

				/// Задать источник и делитель.
				/// \param source Источник тактового сигнала.
				/// \param divider Делитель.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					BindControl::SetPeripheryAuxSignalSource(source);
					BindControl::SetPeripheryAuxSignalDivider(divider);
					return true;
				}

				/// Задать источник и подобрать делитель.
				/// \details Делитель будет выбран на основе указанной частоты источника и желаемой частоты сигнала.
				/// \param source Источник тактового сигнала.
				/// \param sourceFreq Частота источника.
				/// \param signalFreq Желаемая результирующая часто сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source, unsigned long sourceFreq, unsigned long signalFreq)
				{
					BindControl::SetPeripheryAuxSignalSource(source);
					BindControl::SetPeripheryAuxSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

		public:

			///// Определить частоту источника, сравнив её с эталоном.
			///// \param testClockSource Источник, чью частоту хотим измерить.
			///// \param referenceClockSource Источник эталонной частоты.
			///// \param referenceSourceFreq Частота эталонного источника.
			///// \param desiredCalibrationFreq Желаемая эталонная частота во время измерения (чем ниже, тем точнее, грубо говоря).
			//static inline unsigned long CalcFrequency(typename BindControl::ClockSource testClockSource, // источник, чью частоту будем мерить
			//						   	typename BindControl::ClockSource referenceClockSource, // источник эталонной частоты
			//							unsigned long referenceSourceFreq, // значение эталонной частоты
			//							unsigned long desiredCalibrationFreq = 1024) // желаемая частота калибровки (чем ниже, тем точнее, грубо говоря)
			//{
			//	RegisterValueType bindControlReg1 = BindControl::BindRegister();
			//	RegisterValueType bindControlReg2 = BindControl::DividerRegister();
			//	
			//	unsigned long frequency = Msp430::ClockSystemParts::FrequencyCalibration<BindControl, CalibrationParams>::CalcFrequency(
			//											testClockSource,
			//											referenceClockSource,
			//											referenceSourceFreq,
			//											desiredCalibrationFreq);

			//	BindControl::BindRegister() = bindControlReg1;
			//	BindControl::DividerRegister() = bindControlReg2;
			//	
			//	return frequency;
			//}

			/// \brief Проверить и исправить ошибки источников тактовых сигналов.
			/// \details Функция проверяет нет ли установленных флагов ошибок осцилляторов и сбрасывает их, если есть.
			/// Предоставляет возможность истонику, в котором произошла ошибка, проинициализироваться ещё раз.
			/// Если была обнаружена ошибка, генерируется событие EventClockErrorsSet.
			/// Если ошибки были исправлены, генерируется событие EventClockErrorsClear.
			static inline bool CheckErrors(bool generateEvents = true)
			{
				static bool newFault = true;
				bool res = false;
				if (FaultFlagsControl::HasFaults())
				{
				  	FaultFlagsControl::ClearSourceFaults();
					SpecialFunctionRegisters::ClearOscillatorFault();
					
					res = true;
					
					if (newFault && FaultFlagsControl::HasFaults())
					{
						_oscillatorErrorsCounter++;
						newFault = false;
						if (generateEvents)
						{
							Event e(EventClockErrorsSet);
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
					}
				}
				else
				{
					if (newFault == false)
					{
						if (generateEvents)
						{
							Event e(EventClockErrorsClear);
							Singletone<EventDispetcher>::Instance().PushEvent(e);
						}
						newFault = true;
					}
				}
				
				return res;
			}

			/// Возвращает значение счётчика ошибок источников.
			static inline unsigned long FaultsCounter()
			{
				return _oscillatorErrorsCounter;
			}

			protected:

				/// Счётчик ошибок источников.
				static unsigned long _oscillatorErrorsCounter;

		};

		//
		// Инстанцирование статических переменных
		//
		// счётчик ошибок
		template<RegisterAddresType DcoRegister1Address, // UCSCTL0
				RegisterAddresType DcoRegister2Address, // UCSCTL1
				RegisterAddresType FllRegister1Address, // UCSCTL2
				RegisterAddresType FllRegister2Address, // UCSCTL3
				RegisterAddresType BindControlRegister1Address, // UCSCTL4
				RegisterAddresType BindControlRegister2Address, // UCSCTL5
				RegisterAddresType GeneralSettingsRegisterAddress, // UCSCTL6
				RegisterAddresType FaultFlagsRegisterAddress, // UCSCTL7
				class FrequencyRangeSelect,
				class CalibrationParams>
				unsigned long ClockSystem<DcoRegister1Address,
											DcoRegister2Address,
											FllRegister1Address,
											FllRegister2Address,
											BindControlRegister1Address,
											BindControlRegister2Address,
											GeneralSettingsRegisterAddress,
											FaultFlagsRegisterAddress,
											FrequencyRangeSelect,
											CalibrationParams>
		::_oscillatorErrorsCounter = 0;

	}
}
}

#endif // CLOCK_SYSTEM_MSP430F5XXX_H
