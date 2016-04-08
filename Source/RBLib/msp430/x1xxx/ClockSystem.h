///////////////////////////////////////////////////////////////////
//
// Cистема тактирования для семейства msp430x5xxx / msp430x6xxx
//
///////////////////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_MSP430X1XXX_H
#define CLOCK_SYSTEM_MSP430X1XXX_H

#include "../../Common.h"

#include "./ClockSystemClockSources.h"
#include "../x1xxx,x2xxx/ClockSystemClockSignals.h"
#include "../FrequencyCalibration.h"
#include "../../SpecialFunctionRegisters.h"

#include "../../EventDispetcher.h"

namespace Rblib
{
namespace Msp430x1xxx
{
	namespace ClockSystemParts
	{
		/// Тип значения регистров системы тактирования
		typedef unsigned char RegisterValueType;
		/// Тип адреса регистра системы тактирования
		typedef unsigned int RegisterAddresType;

		/// Класс системы тактирования.
		/// \tparam DcoControlRegisterAddress Адрес регистра DCOCTL.
		/// \tparam ControlRegister1Address Адрес регистра BCSCTL1.
		/// \tparam ControlRegister2Address Адрес регистра BCSCTL2.
		/// \tparam ControlRegister3Address Адрес регистра BCSCTL2.
		/// \tparam FrequencyRangeSelect Платформозависимый класс, определяющий в какой диапазон попадает частота.
		/// \tparam CalibrationParams Платформозависимые праметры дял процедуры калибровки.
		template<RegisterAddresType DcoControlRegisterAddress, // DCOCTL
				RegisterAddresType ControlRegister1Address, // BCSCTL1
				RegisterAddresType ControlRegister2Address, // BCSCTL2
				class FrequencyRangeSelect,
				class CalibrationParams>
		class ClockSystem
		{
		public:

			/// Связь источник-сигнал, управление делителями.
      typedef Msp430x12xxx::ClockSystemParts::ClockSourceBindControl<RegisterValueType, RegisterAddresType
							        , ControlRegister1Address, ControlRegister2Address> BindControl;

			//
			// источники
			//

			/// Внешний источник XT1.
			typedef Msp430x12xxx::ClockSystemParts::ExternalClockSource1Impl<RegisterValueType, RegisterAddresType
											, ControlRegister1Address> ExternalClockSource1;

			// Внешний источник XT2.
			typedef Msp430x12xxx::ClockSystemParts::ExternalClockSource2Impl<RegisterValueType, RegisterAddresType
											, ControlRegister1Address> ExternalClockSource2;

			/// Внутренний источник DCO.
			typedef InternalClockSourceImpl<RegisterValueType, RegisterAddresType
											, DcoControlRegisterAddress
											, ControlRegister1Address
											, ControlRegister2Address
											, FrequencyRangeSelect
											, CalibrationParams> InternalClockSource;

			//
			// сигналы
			//

			/// Тактовый сигнал MCLK.
			struct CoreClockSignal : public Msp430x12xxx::ClockSystemParts::CoreClockSignalImpl
			{
				/// Задать источник, установить делитель равным 1.
				/// \param source Источник тактового сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					if (!BindControl::SetCoreSignalSource(source))
						return false;
					BindControl::SetCoreSignalDivider(BindControl::Divider1);
					return true;
				}

				/// Задать источник и делитель.
				/// \param source Источник тактового сигнала.
				/// \param divider Делитель.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					if (!BindControl::SetCoreSignalSource(source))
						return false;
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
					if (!BindControl::SetCoreSignalSource(source))
						return false;
					BindControl::SetCoreSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// Тактовый сигнал SMCLK.
			struct PeripheryClockSignal : public Msp430x12xxx::ClockSystemParts::PeripheryClockSignalImpl
			{
				/// Задать источник, установить делитель равным 1.
				/// \param source Источник тактового сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					if (!BindControl::SetPeripherySignalSource(source))
						return false;
					BindControl::SetPeripherySignalDivider(BindControl::Divider1);
					return true;
				}

				/// Задать источник и делитель.
				/// \param source Источник тактового сигнала.
				/// \param divider Делитель.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					if (!BindControl::SetPeripherySignalSource(source))
						return false;
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
					if (!BindControl::SetPeripherySignalSource(source))
						return false;
					BindControl::SetPeripherySignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// Тактовый сигнал ACLK.
			struct PeripheryAuxClockSignal : public Msp430x12xxx::ClockSystemParts::PeripheryAuxClockSignalImpl
			{
				/// Задать источник, установить делитель равным 1.
				/// \param source Источник тактового сигнала.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					if (!BindControl::SetPeripheryAuxSignalSource(source))
						return false;
					BindControl::SetPeripheryAuxSignalDivider(BindControl::Divider1);
					return true;
				}

				/// Задать источник и делитель.
				/// \param source Источник тактового сигнала.
				/// \param divider Делитель.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					if (!BindControl::SetPeripheryAuxSignalSource(source))
						return false;
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
					if (!BindControl::SetPeripheryAuxSignalSource(source))
						return false;
					BindControl::SetPeripheryAuxSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

		public:

			////
			//// Определить частоту источника
			////
			//static inline unsigned long CalcFrequency(typename BindControl::ClockSource testClockSource, // источник, чью частоту будем мерить
			//						   	typename BindControl::ClockSource referenceClockSource, // источник эталонной частоты
			//							unsigned long referenceSourceFreq, // значение эталонной частоты
			//							unsigned long desiredCalibrationFreq = 1024) // желаемая частота калибровки (чем ниже, тем точнее, грубо говоря)
			//{
			//	RegisterValueType bindControlReg1 = BindControl::BindRegister();
			//	RegisterValueType bindControlReg2 = BindControl::DividerRegister();
			//	
			//	unsigned long frequency = FrequencyCalibration<BindControl, CalibrationParams>::CalcFrequency(
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
			static inline void CheckErrors()
			{
				//static bool newFault = true;
				//if (FaultFlagsControl::HasFaults())
				//{
				//	SpecialFunctionRegisters::ClearOscillatorFault();
				//  	FaultFlagsControl::ClearSourceFaults();
				//	if (newFault && FaultFlagsControl::HasFaults())
				//	{
				//		_oscillatorErrorsCounter++;
				//		newFault = false;
				//		Event e(EventClockErrorsSet);
				//		Singletone<EventDispetcher>::Instance().PushEvent(e);
				//	}
				//}
				//else
				//{
				//	if (newFault == false)
				//	{
				//		Event e(EventClockErrorsClear);
				//		Singletone<EventDispetcher>::Instance().PushEvent(e);
				//		newFault = true;
				//	}
				//}
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
		template<RegisterAddresType DcoControlRegisterAddress, // DCOCTL
				RegisterAddresType ControlRegister1Address, // BCSCTL1
				RegisterAddresType ControlRegister2Address, // BCSCTL2
				class FrequencyRangeSelect,
				class CalibrationParams>
				unsigned long ClockSystem<DcoControlRegisterAddress,
											ControlRegister1Address,
											ControlRegister2Address,
											FrequencyRangeSelect,
											CalibrationParams>
		::_oscillatorErrorsCounter = 0;

	}
}
}

#endif // CLOCK_SYSTEM_MSP430X1XXX_H
