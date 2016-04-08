///////////////////////////////////////////////////////////////////
//
// C������ ������������ ��� ��������� msp430x5xxx / msp430x6xxx
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
		/// ��� �������� ��������� ������� ������������
		typedef unsigned char RegisterValueType;
		/// ��� ������ �������� ������� ������������
		typedef unsigned int RegisterAddresType;

		/// ����� ������� ������������.
		/// \tparam DcoControlRegisterAddress ����� �������� DCOCTL.
		/// \tparam ControlRegister1Address ����� �������� BCSCTL1.
		/// \tparam ControlRegister2Address ����� �������� BCSCTL2.
		/// \tparam ControlRegister3Address ����� �������� BCSCTL2.
		/// \tparam FrequencyRangeSelect ������������������ �����, ������������ � ����� �������� �������� �������.
		/// \tparam CalibrationParams ������������������ �������� ��� ��������� ����������.
		template<RegisterAddresType DcoControlRegisterAddress, // DCOCTL
				RegisterAddresType ControlRegister1Address, // BCSCTL1
				RegisterAddresType ControlRegister2Address, // BCSCTL2
				class FrequencyRangeSelect,
				class CalibrationParams>
		class ClockSystem
		{
		public:

			/// ����� ��������-������, ���������� ����������.
      typedef Msp430x12xxx::ClockSystemParts::ClockSourceBindControl<RegisterValueType, RegisterAddresType
							        , ControlRegister1Address, ControlRegister2Address> BindControl;

			//
			// ���������
			//

			/// ������� �������� XT1.
			typedef Msp430x12xxx::ClockSystemParts::ExternalClockSource1Impl<RegisterValueType, RegisterAddresType
											, ControlRegister1Address> ExternalClockSource1;

			// ������� �������� XT2.
			typedef Msp430x12xxx::ClockSystemParts::ExternalClockSource2Impl<RegisterValueType, RegisterAddresType
											, ControlRegister1Address> ExternalClockSource2;

			/// ���������� �������� DCO.
			typedef InternalClockSourceImpl<RegisterValueType, RegisterAddresType
											, DcoControlRegisterAddress
											, ControlRegister1Address
											, ControlRegister2Address
											, FrequencyRangeSelect
											, CalibrationParams> InternalClockSource;

			//
			// �������
			//

			/// �������� ������ MCLK.
			struct CoreClockSignal : public Msp430x12xxx::ClockSystemParts::CoreClockSignalImpl
			{
				/// ������ ��������, ���������� �������� ������ 1.
				/// \param source �������� ��������� �������.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					if (!BindControl::SetCoreSignalSource(source))
						return false;
					BindControl::SetCoreSignalDivider(BindControl::Divider1);
					return true;
				}

				/// ������ �������� � ��������.
				/// \param source �������� ��������� �������.
				/// \param divider ��������.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					if (!BindControl::SetCoreSignalSource(source))
						return false;
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
					if (!BindControl::SetCoreSignalSource(source))
						return false;
					BindControl::SetCoreSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// �������� ������ SMCLK.
			struct PeripheryClockSignal : public Msp430x12xxx::ClockSystemParts::PeripheryClockSignalImpl
			{
				/// ������ ��������, ���������� �������� ������ 1.
				/// \param source �������� ��������� �������.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					if (!BindControl::SetPeripherySignalSource(source))
						return false;
					BindControl::SetPeripherySignalDivider(BindControl::Divider1);
					return true;
				}

				/// ������ �������� � ��������.
				/// \param source �������� ��������� �������.
				/// \param divider ��������.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					if (!BindControl::SetPeripherySignalSource(source))
						return false;
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
					if (!BindControl::SetPeripherySignalSource(source))
						return false;
					BindControl::SetPeripherySignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

			/// �������� ������ ACLK.
			struct PeripheryAuxClockSignal : public Msp430x12xxx::ClockSystemParts::PeripheryAuxClockSignalImpl
			{
				/// ������ ��������, ���������� �������� ������ 1.
				/// \param source �������� ��������� �������.
				static inline bool SetSource(typename BindControl::ClockSource source)
				{
					if (!BindControl::SetPeripheryAuxSignalSource(source))
						return false;
					BindControl::SetPeripheryAuxSignalDivider(BindControl::Divider1);
					return true;
				}

				/// ������ �������� � ��������.
				/// \param source �������� ��������� �������.
				/// \param divider ��������.
				static inline bool SetSource(typename BindControl::ClockSource source, typename BindControl::Divider divider)
				{
					if (!BindControl::SetPeripheryAuxSignalSource(source))
						return false;
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
					if (!BindControl::SetPeripheryAuxSignalSource(source))
						return false;
					BindControl::SetPeripheryAuxSignalDivider(sourceFreq / signalFreq);
					return true;
				}
			};

		public:

			////
			//// ���������� ������� ���������
			////
			//static inline unsigned long CalcFrequency(typename BindControl::ClockSource testClockSource, // ��������, ��� ������� ����� ������
			//						   	typename BindControl::ClockSource referenceClockSource, // �������� ��������� �������
			//							unsigned long referenceSourceFreq, // �������� ��������� �������
			//							unsigned long desiredCalibrationFreq = 1024) // �������� ������� ���������� (��� ����, ��� ������, ����� ������)
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


			/// \brief ��������� � ��������� ������ ���������� �������� ��������.
			/// \details ������� ��������� ��� �� ������������� ������ ������ ������������ � ���������� ��, ���� ����.
			/// ������������� ����������� ��������, � ������� ��������� ������, ��������������������� ��� ���.
			/// ���� ���� ���������� ������, ������������ ������� EventClockErrorsSet.
			/// ���� ������ ���� ����������, ������������ ������� EventClockErrorsClear.
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
