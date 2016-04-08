///////////////////////////////////////////////////////////////////
//
// C������ ������������ ��� ��������� msp430x5xxx / msp430x6xxx
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
		/// ��� �������� ��������� ������� ������������
		typedef unsigned short RegisterValueType;
		/// ��� ������ �������� ������� ������������
		typedef unsigned int RegisterAddresType;

		/// \brief ����� ������� ������������.
		/// \tparam DcoRegister1Address ����� �������� UCSCTL0
		/// \tparam DcoRegister2Address ����� �������� UCSCTL1
		/// \tparam FllRegister1Address ����� �������� 
		/// \tparam FllRegister2Address ����� �������� UCSCTL3
		/// \tparam BindControlRegister1Address ����� �������� UCSCTL4
		/// \tparam BindControlRegister2Address ����� �������� UCSCTL5
		/// \tparam GeneralSettingsRegisterAddress ����� �������� UCSCTL6
		/// \tparam FaultFlagsRegisterAddress ����� �������� UCSCTL7
		/// \tparam FrequencyRangeSelect ������������������ �����, ������������ � ����� �������� �������� �������.
		/// \tparam CalibrationParams ������������������ �������� ��� ��������� ����������
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

			/// ����� ��������-������, ���������� ����������.
			typedef ClockSourceBindControl<RegisterValueType, RegisterAddresType
										, BindControlRegister1Address, BindControlRegister2Address> BindControl;

			/// ������ � ������ ������ �����������.
			typedef ClockSourceFaultFlagsControl<RegisterValueType, RegisterAddresType
											, FaultFlagsRegisterAddress> FaultFlagsControl;			

			//
			// ���������
			//

			/// ������� �������� XT1.
			typedef ExternalClockSource1Impl<RegisterValueType, RegisterAddresType
											, GeneralSettingsRegisterAddress> ExternalClockSource1;

			/// ������� �������� XT2.
			typedef ExternalClockSource2Impl<RegisterValueType, RegisterAddresType
											, GeneralSettingsRegisterAddress> ExternalClockSource2;

			/// ���������� �������� DCO.
			typedef InternalClockSourceImpl<RegisterValueType, RegisterAddresType
											, DcoRegister1Address, DcoRegister2Address
											, FllRegister1Address, FllRegister2Address
											, BindControlRegister1Address, BindControlRegister2Address
											, FrequencyRangeSelect
											, CalibrationParams> InternalClockSource;

			/// ���������� �������� DCODIV.
			typedef InternalDivClockSourceImpl<RegisterValueType, RegisterAddresType
											, FllRegister1Address, FllRegister2Address> InternalDivClockSource;

			/// ���������� �������� REFOCLK.
			typedef InternalLowFreqClockSource1Impl InternalLowFreqClockSource1;

			/// ���������� �������� VLOCLK.
			typedef InternalLowFreqClockSource2Impl InternalLowFreqClockSource2;

			//
			// �������
			//

			/// �������� ������ MCLK.
			struct CoreClockSignal : public CoreClockSignalImpl
			{
				/// ������ ��������, ���������� �������� ������ 1.
				/// \param source �������� ��������� �������.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					BindControl::SetCoreSignalSource(source);
					BindControl::SetCoreSignalDivider(BindControl::Divider1);
					return true;
				}

				/// ������ �������� � ��������.
				/// \param source �������� ��������� �������.
				/// \param divider ��������.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					BindControl::SetCoreSignalSource(source);
					BindControl::SetCoreSignalDivider(divider);
					return true;
				}

				/// ������ �������� � ��������� ��������.
				/// \details �������� ����� ������ �� ������ ��������� ������� ��������� � �������� ������� �������.
				/// \param source �������� ��������� �������.
				/// \param sourceFreq ������� ���������.
				/// \param signalFreq �������� �������������� ����� �������.
				static inline bool SetSource(typename BindControl::ClockSource source, unsigned long sourceFreq, unsigned long signalFreq)
				{
					BindControl::SetCoreSignalSource(source);
					BindControl::SetCoreSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// �������� ������ SMCLK.
			struct PeripheryClockSignal : public PeripheryClockSignalImpl
			{
				/// ������ ��������, ���������� �������� ������ 1.
				/// \param source �������� ��������� �������.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					BindControl::SetPeripherySignalSource(source);
					BindControl::SetPeripherySignalDivider(BindControl::Divider1);
					return true;
				}

				/// ������ �������� � ��������.
				/// \param source �������� ��������� �������.
				/// \param divider ��������.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					BindControl::SetPeripherySignalSource(source);
					BindControl::SetPeripherySignalDivider(divider);
					return true;
				}

				/// ������ �������� � ��������� ��������.
				/// \details �������� ����� ������ �� ������ ��������� ������� ��������� � �������� ������� �������.
				/// \param source �������� ��������� �������.
				/// \param sourceFreq ������� ���������.
				/// \param signalFreq �������� �������������� ����� �������.
				static inline bool SetSource(typename BindControl::ClockSource source, unsigned long sourceFreq, unsigned long signalFreq)
				{
					BindControl::SetPeripherySignalSource(source);
					BindControl::SetPeripherySignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// �������� ������ ACLK.
			struct PeripheryAuxClockSignal : public PeripheryAuxClockSignalImpl
			{
				/// ������ ��������, ���������� �������� ������ 1.
				/// \param source �������� ��������� �������.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					BindControl::SetPeripheryAuxSignalSource(source);
					BindControl::SetPeripheryAuxSignalDivider(BindControl::Divider1);
					return true;
				}

				/// ������ �������� � ��������.
				/// \param source �������� ��������� �������.
				/// \param divider ��������.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					BindControl::SetPeripheryAuxSignalSource(source);
					BindControl::SetPeripheryAuxSignalDivider(divider);
					return true;
				}

				/// ������ �������� � ��������� ��������.
				/// \details �������� ����� ������ �� ������ ��������� ������� ��������� � �������� ������� �������.
				/// \param source �������� ��������� �������.
				/// \param sourceFreq ������� ���������.
				/// \param signalFreq �������� �������������� ����� �������.
				static inline bool SetSource(typename BindControl::ClockSource source, unsigned long sourceFreq, unsigned long signalFreq)
				{
					BindControl::SetPeripheryAuxSignalSource(source);
					BindControl::SetPeripheryAuxSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

		public:

			///// ���������� ������� ���������, ������� � � ��������.
			///// \param testClockSource ��������, ��� ������� ����� ��������.
			///// \param referenceClockSource �������� ��������� �������.
			///// \param referenceSourceFreq ������� ���������� ���������.
			///// \param desiredCalibrationFreq �������� ��������� ������� �� ����� ��������� (��� ����, ��� ������, ����� ������).
			//static inline unsigned long CalcFrequency(typename BindControl::ClockSource testClockSource, // ��������, ��� ������� ����� ������
			//						   	typename BindControl::ClockSource referenceClockSource, // �������� ��������� �������
			//							unsigned long referenceSourceFreq, // �������� ��������� �������
			//							unsigned long desiredCalibrationFreq = 1024) // �������� ������� ���������� (��� ����, ��� ������, ����� ������)
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

			/// \brief ��������� � ��������� ������ ���������� �������� ��������.
			/// \details ������� ��������� ��� �� ������������� ������ ������ ������������ � ���������� ��, ���� ����.
			/// ������������� ����������� ��������, � ������� ��������� ������, ��������������������� ��� ���.
			/// ���� ���� ���������� ������, ������������ ������� EventClockErrorsSet.
			/// ���� ������ ���� ����������, ������������ ������� EventClockErrorsClear.
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

			/// ���������� �������� �������� ������ ����������.
			static inline unsigned long FaultsCounter()
			{
				return _oscillatorErrorsCounter;
			}

			protected:

				/// ������� ������ ����������.
				static unsigned long _oscillatorErrorsCounter;

		};

		//
		// ��������������� ����������� ����������
		//
		// ������� ������
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
