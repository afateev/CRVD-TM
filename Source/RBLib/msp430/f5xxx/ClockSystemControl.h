///////////////////////////////////////////////////
//
// ������ ������� � ��������� ������� ������������
//
///////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_CONTROL_H
#define CLOCK_SYSTEM_CONTROL_H

#include "ClockSystemRegisters.h"

namespace Rblib
{
namespace Msp430f5xxx
{
	namespace ClockSystemParts
	{
		/// ����� ���������� ���������� DCO (UCSCTL0, UCSCTL1).
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� UCSCTL0.
		/// \tparam ControlRegister2Address ����� �������� UCSCTL1.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class DcoControl
		{
		public:

			/// ���������.
			enum Constants
			{
				/// ������������� �������� �������� ������������ �������� (UCSCTL0, ���� DCO + MOD).
				MaxUnifiedTapSelect = 0x3FF,
			};

		protected:

			/// ��������� �� ������� UCSCTL0.
			static volatile DcoControlRegister1* const _controlRegister1Addr;
			/// ��������� �� ������� UCSCTL1.
			static volatile DcoControlRegister2* const _controlRegister2Addr;

		public:
			
			/// ���������� ���������� ���������� �������.
			/// \details ������������� �������� ���� DCORSEL � �������� UCSCTL1.
			static inline unsigned char MaxFrequencyRange()
			{
				return 7;
			}

			/// ��������� �� ������� UCSCTL0.
			static inline RegisterDataType& DcoRegister1()
			{
				return (RegisterDataType&) *_controlRegister1Addr;
			}

			/// ��������� �� ������� UCSCTL1.
			static inline RegisterDataType& DcoRegister2()
			{
				return (RegisterDataType&) *_controlRegister2Addr;
			}

			/// �������� ���������.
			static inline void EnableModulation()
			{
				_controlRegister2Addr->ModulationEnable = 0;
			}

			/// ��������� ���������.
			static inline void DisableModulation()
			{
				_controlRegister2Addr->ModulationEnable = 1;
			}

			/// ���������� �������� ���� MOD �������� UCSCTL0.
			static inline RegisterDataType GetModulationCounter()
			{
				return _controlRegister1Addr->ModulationCounter;
			}

			/// ������������� �������� ���� MOD �������� UCSCTL0.
			static inline void SetModulationCounter(RegisterDataType modulationCounter)
			{
				_controlRegister1Addr->ModulationCounter = modulationCounter;
			}

			/// ���������� �������� ���� DCO �������� UCSCTL0.
			static inline RegisterDataType GetTapSelect()
			{
				return _controlRegister1Addr->TapSelect;
			}

			/// ������������� �������� ���� DCO �������� UCSCTL0.
			static inline void SetTapSelect(RegisterDataType tapSelect)
			{
				_controlRegister1Addr->TapSelect = tapSelect;
			}

			/// ���������� ���� DCO � MOD �������� UCSCTL0, ��� ���� ��������.
			static inline RegisterDataType GetUnifiedTapSelect()
			{
				return _controlRegister1Addr->UnifiedTapField;
			}

			/// ������������� ���� DCO � MOD �������� UCSCTL0, ��� ���� ��������.
			static inline void SetUnifiedTapSelect(RegisterDataType tapSelect)
			{
				_controlRegister1Addr->UnifiedTapField = tapSelect;
			}

			/// ����������� �� 1 ����������� ���� DCO + MOD.
			static inline void IncUnifiedTapSelect()
			{
				_controlRegister1Addr->UnifiedTapField++;
			}

			/// ��������� �� 1 ����������� ���� DCO + MOD.
			static inline void DecUnifiedTapSelect()
			{
				_controlRegister1Addr->UnifiedTapField--;
			}

			/// ���������� ������� ���� DCORSEL �������� UCSCTL1.
			static inline RegisterDataType GetFrequencyRange()
			{
				return _controlRegister2Addr->FrequencyRange;
			}

			/// ������������� ������� ���� DCORSEL �������� UCSCTL1.
			static inline void SetFrequencyRange(RegisterDataType frequencyRange)
			{
				_controlRegister2Addr->FrequencyRange = frequencyRange;
			}

			/// ����������� �� 1 ������� ���� DCORSEL �������� UCSCTL1.
			static inline void IncFrequencyRange()
			{
				_controlRegister2Addr->FrequencyRange++;
			}

			/// ��������� �� 1 ������� ���� DCORSEL �������� UCSCTL1.
			static inline void DecFrequencyRange()
			{
				_controlRegister2Addr->FrequencyRange--;
			}
		};

		/// ����� ���������� ���������� FLL (UCSCTL2, UCSCTL3)
		/// \details FLL ������������ ��� �������������� ���������� ������� DCO � �������� �������.
		/// \details �������� �������, ������� ����� ������������ FLL, ������������ �������� ���������� �������, 
		/// \details ��������� ������� ���������� �������, � ������ ����������� FLL: ����������� � ���������.
		/// \details Fdco = Dfll * (Mfll + 1) * (Fref / Dref)\n
		/// \details Fdcodv = (Mfll + 1) * (Fref / Dref)\n
		/// \details Dfll - �������� Fll.
		/// \details Mfll - ��������� Fll.
		/// \details Fref - ������� ���������� �������.
		/// \details Dref - �������� ������� ���������� �������.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� UCSCTL2.
		/// \tparam ControlRegister2Address ����� �������� UCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class FllControl
		{
		public:

			/// ��������� ��������� ���������.
			enum FllReferenceSelect
			{
				/// ������� �������� XT1.
				FllReferenceSelectExternlaClock1 = 0,
				/// ������� �������� XT2.
				FllReferenceSelectExternlaClock2 = 5,
				/// ���������� REFOCLK.
				FllReferenceSelectInternalLowFreqClock = 2,
				/// �������� �� ������.
				FllReferenceSelectNone = 7
			};

			/// �������� ��������� �������.
			enum FllReferenceDivider
			{
				/// 1
				FllReferenceDivider1 = 0,
				/// 2
				FllReferenceDivider2 = 1,
				/// 3
				FllReferenceDivider4 = 2,
				/// 8
				FllReferenceDivider8 = 3,
				/// 12
				FllReferenceDivider12 = 4,
				/// 16
				FllReferenceDivider16 = 5,
			};

			/// �������� Fll (Dfll).
			enum FllDivider
			{
				/// 1
				FllDivider1 = 0,
				/// 2
				FllDivider2 = 1,
				/// 4
				FllDivider4 = 2,
				/// 8
				FllDivider8 = 3,
				/// 16
				FllDivider16 = 4,
				/// 32
				FllDivider32 = 5,
			};

			/// ���������.
			enum Consants
			{
				/// ������������ �������� ��������� Fll (Mfll).
				FllMaxMultiplierValue = 1023
			};


		protected:

			/// ��������� �� ������� UCSCTL2.
			static volatile FllControlRegister1 * const _controlRegister1Addr;
			/// ��������� �� ������� UCSCTL3.
			static volatile FllControlRegister2 * const _controlRegister2Addr;

		public:

			/// ������ ������ � �������� UCSCTL2.
			static inline RegisterDataType& FllRegister1()
			{
				return *_controlRegister1Addr;
			}

			/// ������ ������ � �������� UCSCTL3.
			static inline RegisterDataType& FllRegister2()
			{
				return *_controlRegister2Addr;
			}
			
			/// ���������� �������� �������� Fll.
			static inline FllDivider GetFllDivider()
			{
				return (FllDivider) _controlRegister1Addr->Divider;
			}

			/// ������������� �������� �������� Fll.
			/// \param divider �������� ��������.
			static inline void SetFllDivider(FllDivider divider)
			{
				_controlRegister1Addr->Divider = divider;
			}

			/// ���������� �������� ��������� Fll.
			static inline RegisterDataType GetFllMultiplier()
			{
				return _controlRegister1Addr->Multiplier;
			}

			/// ������������� �������� ��������� Fll.
			/// \param multiplier �������� ���������.
			static inline void SetFllMultiplier(RegisterDataType multiplier)
			{
				_controlRegister1Addr->Multiplier = multiplier;
			}

			/// ������ �������� ��������� �������.
			/// \param reference �������� ��������� �������.
			static inline void SetFllReference(FllReferenceSelect reference)
			{
				_controlRegister2Addr->ReferenceSelect = reference;
			}

			/// ������ �������� ��������� �������.
			/// \param divider �������� ��������� ��������� �������.
			static inline void SetFllReferenceDivider(FllReferenceDivider divider)
			{
				_controlRegister2Addr->ReferenceDivider = divider;
			}

		public:

			/// ������������� �������� Fll �� ����� � �����.
			static inline unsigned char fllDividerToValue(FllDivider fllDivider)
			{
				switch(fllDivider)
				{
				case FllDivider1:
					return 1;
				case FllDivider2:
					return 2;
				case FllDivider4:
					return 4;
				case FllDivider8:
					return 8;
				case FllDivider16:
					return 16;
				case FllDivider32:
					return 32;
				}
				return 1;
			}

			/// ������������� �������� Fll �� � ����� � ����.
			static inline FllDivider valueToFllDivider(unsigned char divider)
			{
				if (divider >= 32)
					return FllDivider32;
				else if (divider >= 16)
					return FllDivider16;
				else if (divider >= 8)
					return FllDivider8;
				else if (divider >= 4)
					return FllDivider4;
				else if (divider >= 2)
					return FllDivider2;
				else if (divider >= 1)
					return FllDivider1;
				else
					return FllDivider1;
			}

			/// ������������� �������� ��������� ������� �� ����� � �����.
			static inline unsigned char fllReferenceDividerToValue(FllReferenceDivider fllRefDivider)
			{
				switch(fllRefDivider)
				{
				case FllReferenceDivider1:
					return 1;
				case FllReferenceDivider2:
					return 2;
				case FllReferenceDivider4:
					return 4;
				case FllReferenceDivider8:
					return 8;
				case FllReferenceDivider12:
					return 12;
				case FllReferenceDivider16:
					return 16;
				}
				return 1;
			}

			/// ������������� �������� ��������� ������� �� � ����� � ����.
			static inline FllReferenceDivider valueToFllReferenceDivider(unsigned char divider)
			{
				if (divider >= 16)
					return FllReferenceDivider16;
				else if (divider >= 12)
					return FllReferenceDivider12;
				else if (divider >= 8)
					return FllReferenceDivider8;
				else if (divider >= 4)
					return FllReferenceDivider4;
				else if (divider >= 2)
					return FllReferenceDivider2;
				else if (divider >= 1)
					return FllReferenceDivider1;
				else
					return FllReferenceDivider1;
			}
		};

		/// ����� ��������������� ������ �������� ��� ������������� ���������� ������������
		/// \details ����������� � ������������� ������ ������ ���������� ������������� �� ClockSourceNamesProvider
		class ClockSourceNamesProvider
		{
		public:

			/// ������ �������� ���������������� ���������.
			enum ClockSource
			{
				/// XT1
				ClockSourceExternal1 = 0,
				/// VLOCLK
				ClockSourceInternalLowFreq2 = 1,
				/// REFOCLK
				ClockSourceInternalLowFreq1 = 2,
				/// DCO
				ClockSourceInternal = 3,
				/// DCODIV
				ClockSourceInternalDiv = 4,
				/// XT2
				ClockSourceExternal2 = 5
			};

		};

		/// ����� ������� � �������� �������� ��������� � �������� � �������� ��������� ��������� (UCSCTL4, UCSCTL5)
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� UCSCTL4.
		/// \tparam ControlRegister2Address ����� �������� UCSCTL5.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class ClockSourceBindControl : virtual public ClockSourceNamesProvider
		{

		public:

			/// ��������.
			enum Divider
			{
				/// 1
				Divider1 = 0,
				/// 2
				Divider2 = 1,
				/// 4
				Divider4 = 2,
				/// 8
				Divider8 = 3,
				/// 16
				Divider16 = 4,
				/// 32
				Divider32 = 5,
			};

		protected:

			/// ��������� �� ������� UCSCTL4.
			static volatile ClockSourceSelectRegister * const _bindControlRegisterAddr;
			/// ��������� �� ������� UCSCTL5.
			static volatile ClockDividerRegister * const _dividerControlRegisterAddr;

		public:

			/// ����������� �������� �������� �� ����� � ����, �������� � �������� �������.
			static inline Divider GetDivider(unsigned char divider)
			{
				if (divider >= 32)
					return Divider32;
				else if (divider >= 16)
					return Divider16;
				else if (divider >= 8)
					return Divider8;
				else if (divider >= 4)
					return Divider4;
				else if (divider >= 2)
					return Divider2;
				else if (divider >= 1)
					return Divider1;
				else
					return Divider1;
			}

			/// ����������� �������� �������� �� ����� � ����, �������� � ������� �������.
			static inline Divider GetUpDivider(unsigned char divider)
			{
				if (divider > 16)
					return Divider32;
				else if (divider > 8)
					return Divider16;
				else if (divider > 4)
					return Divider8;
				else if (divider > 2)
					return Divider4;
				else if (divider > 1)
					return Divider2;
				else
					return Divider1;
			}

			/// ����������� �������� �������� �� ����� � �����.
			static inline unsigned char GetDividerValue(Divider divider)
			{
				switch(divider)
				{
				case Divider1:
					return 1;
				case Divider2:
					return 2;
				case Divider4:
					return 4;
				case Divider8:
					return 8;
				case Divider16:
					return 16;
				case Divider32:
					return 32;
				}
					return 1;
			}

		public:

			/// ������ � �������� UCSCTL4
			static inline RegisterDataType& BindRegister()
			{
				return (RegisterDataType&) *_bindControlRegisterAddr;
			}

			/// ������ � �������� UCSCTL5
			static inline RegisterDataType& DividerRegister()
			{
				return (RegisterDataType&) *_dividerControlRegisterAddr;
			}

			// ���������

			/// ���������� �������� ��� ������� MCLK.
			static inline void SetCoreSignalSource(ClockSource source)
			{
				_bindControlRegisterAddr->CoreClockSource = source;
			}

			/// ���������� �������� ��� ������� SMCLK.
			static inline void SetPeripherySignalSource(ClockSource source)
			{
				_bindControlRegisterAddr->PeripheryClockSource = source;
			}
			
			/// ���������� �������� ��� ������� ACLK.
			static inline void SetPeripheryAuxSignalSource(ClockSource source)
			{
				_bindControlRegisterAddr->PeripheryAuxClockSource = source;
			}

			// ��������

			/// ���������� �������� ��� ������� MCLK.
			static inline void SetCoreSignalDivider(Divider divider)
			{
				_dividerControlRegisterAddr->CoreClockDivider = divider;
			}

			/// ���������� �������� ��� ������� SMCLK.
			static inline void SetPeripherySignalDivider(Divider divider)
			{
				_dividerControlRegisterAddr->PeripheryClockDivider = divider;
			}
			
			/// ���������� �������� ��� ������� ACLK.
			static inline void SetPeripheryAuxSignalDivider(Divider divider)
			{
				_dividerControlRegisterAddr->PeripheryAuxClockDivider = divider;
			}

			/// ���������� �������� ��� ������� MCLK.
			static inline void SetCoreSignalDivider(unsigned long divider)
			{
				_dividerControlRegisterAddr->CoreClockDivider = GetDivider(divider);
			}

			/// ���������� �������� ��� ������� SMCLK.
			static inline void SetPeripherySignalDivider(unsigned long divider)
			{
				_dividerControlRegisterAddr->PeripheryClockDivider = GetDivider(divider);
			}
			
			/// ���������� �������� ��� ������� ACLK.
			static inline void SetPeripheryAuxSignalDivider(unsigned long divider)
			{
				_dividerControlRegisterAddr->PeripheryAuxClockDivider = GetDivider(divider);
			}
		};

		/// ����� ���������� ��������� ����� �������� ���������� ������������ (UCSCTL6).
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� UCSCTL6.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class GeneralSettingsControl
		{
		public:

			/// ������������ ���������� ������������ ���.
			/// \details �� ���������� ������ ������� ��������� ������ ���������.
			enum CurrentAdjust
			{
				/// ����� ������ ����������������� � ������ LF. ���������� ������� � ������ HF 4-8 Mhz.
				CurrentAdjustLevel1 = 0,
				/// ���������� ��������� � ������ LF. ���������� ������� � ������ HF 8-16 Mhz.
				CurrentAdjustLevel2 = 1,
				/// ���������� ��������� � ������ LF. ���������� ������� � ������ HF 16-24 Mhz.
				CurrentAdjustLevel3 = 2,
				/// ������������ ��������� � ������������ ���� � ������ LF. ���������� ������� � ������ HF 24-32 Mhz.
				CurrentAdjustLevel4 = 3
			};

			/// ��������� �������� ��������.
			enum Capasitor
			{
				/// 2��
				Capasitor2Pf = 0,
				/// 6��
				Capasitor6Pf = 1,
				/// 9��
				Capasitor9Pf = 2,
				/// 12��
				Capasitor12Pf = 3
			};

		protected:

			/// ��������� �� ������� UCSCTL6.
			static volatile GeneralSettingsRegister * const _generalSettingsRegisterAddr;

		public:
			
			/// �������� �������� XT1.
			static inline void ExternalClock1On()
			{
				_generalSettingsRegisterAddr->ExternalSource1Off = 0;
			}

			/// ��������� �������� XT1.
			static inline void ExternalClock1Off()
			{
				_generalSettingsRegisterAddr->ExternalSource1Off = 1;
			}

			/// ���������� ������� ����������������� ��������� XT1.
			static inline void SetExternalClock1CurrentAdjust(CurrentAdjust currentAdjust)
			{
				_generalSettingsRegisterAddr->ExternalSource1CurrentAdjust = currentAdjust;
			}

			/// �������� ����� ������� �������.
			static inline void ExternalClock1HighFreqModeEnable()
			{
				_generalSettingsRegisterAddr->ExternalSource1HighFrequencyModeEnable = 1;
			}

			/// ��������� ����� ������� �������.
			static inline void ExternalClock1HighFreqModeDisable()
			{
				_generalSettingsRegisterAddr->ExternalSource1HighFrequencyModeEnable = 0;
			}

			/// ��������� ������������ ������� �������� ������, �������� �� ���� XT1.
			static inline void ExternalClock1PinModeEnable()
			{
				_generalSettingsRegisterAddr->ExternalSource1PinSourceEnable = 1;
			}

			/// ��������� ������������ ������� �������� ������, �������� �� ���� XT1.
			static inline void ExternalClock1PinModeDisable()
			{
				_generalSettingsRegisterAddr->ExternalSource1PinSourceEnable = 0;
			}
			
			/// ���������� �������� ���������� ������� ��� XT1.
			static inline void SetExternalClock1Capasitor(Capasitor capasitor)
			{
				_generalSettingsRegisterAddr->ExternalSource1CapacitorSelect = capasitor;
			}

			/// �������� �������� XT2.
			static inline void ExternalClock2On()
			{
				_generalSettingsRegisterAddr->ExternalSource2Off = 0;
			}

			/// ��������� �������� XT1.
			static inline void ExternalClock2Off()
			{
				_generalSettingsRegisterAddr->ExternalSource2Off = 1;
			}

			/// ���������� ������� ����������������� ��������� XT2.
			static inline void SetExternalClock2CurrentAdjust(CurrentAdjust currentAdjust)
			{
				_generalSettingsRegisterAddr->ExternalSource2CurrentAdjust = currentAdjust;
			}

			/// ��������� ������������ ������� �������� ������, �������� �� ���� XT1.
			static inline void ExternalClock2PinModeEnable()
			{
				_generalSettingsRegisterAddr->ExternalSource2PinSourceEnable = 1;
			}

			/// ��������� ������������ ������� �������� ������, �������� �� ���� XT2.
			static inline void ExternalClock2PinModeDisable()
			{
				_generalSettingsRegisterAddr->ExternalSource2PinSourceEnable = 0;
			}

			/// �������� SMCLK.
			static inline void PeripheryClockSignalOn()
			{
				_generalSettingsRegisterAddr->PeripheryClockSignalOff = 0;
			}

			/// ��������� SMCLK.
			static inline void PeripheryClockSignalOff()
			{
				_generalSettingsRegisterAddr->PeripheryClockSignalOff = 1;
			}
		};

		/// ������ � ������ ������ ����������� (UCSCTL7).
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� UCSCTL7.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ClockSourceFaultFlagsControl
		{
		protected:

			/// ��������� �� ������� UCSCTL7.
			static volatile ClockSourcFaultFlagsRegister * const _faultFlagsRegisterAddr;

		public:

			/// ������ � �������� UCSCTL7.
			static inline RegisterDataType& FaultFlagsRegister()
			{
				return (RegisterDataType&) *_faultFlagsRegisterAddr;
			}

			/// ��������� ������� ����� ������ DCO.
			static inline bool InternalSourceFault()
			{
				return _faultFlagsRegisterAddr->InternalSourceFault;
			}

			/// ��������� ������� ����� ����� XT1 � ������ ������ �������.
			static inline bool ExternalSource1LowFreqFault()
			{
				return _faultFlagsRegisterAddr->ExternalSource1LowFreqFault;
			}

			/// ��������� ������� ����� ����� XT1 � ������ ������� �������.
			static inline bool ExternalSource1HighFreqFault()
			{
				return _faultFlagsRegisterAddr->ExternalSource1HighFreqFault;
			}

			/// ��������� ������� ����� ����� XT2.
			static inline bool ExternalSource2Fault()
			{
				return _faultFlagsRegisterAddr->ExternalSource2Fault;
			}

			/// ��������� ������� �����-���� ������.
			static inline bool HasFaults()
			{
				return _faultFlagsRegisterAddr->SourceFaults;
			}

			/// ����� ����� ������ DCO.
			static inline void ClearInternalSourceFault()
			{
				_faultFlagsRegisterAddr->InternalSourceFault = 0;
			}

			/// ����� ����� ������ XT1 � ������ ������ ������.
			static inline void ClearExternalSource1LowFreqFault()
			{
				_faultFlagsRegisterAddr->ExternalSource1LowFreqFault = 0;
			}

			/// ����� ����� ������ XT1 � ������ ������� ������.
			static inline void ClearExternalSource1HighFreqFault()
			{
				_faultFlagsRegisterAddr->ExternalSource1HighFreqFault = 0;
			}

			/// ����� ����� ������ XT2.
			static inline void ClearExternalSource2Fault()
			{
				_faultFlagsRegisterAddr->ExternalSource2Fault = 0;
			}

			/// ����� ���� ������ ������.
			static inline void ClearSourceFaults()
			{
				_faultFlagsRegisterAddr->SourceFaults = 0;
			}

			/// �������� ��� ����� ������.
			static inline RegisterDataType GetSourceFaults()
			{
				return _faultFlagsRegisterAddr->SourceFaults;
			}

		};

		//
		// ��������������� ����������� ����������
		//
		// DcoControl
		//
		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address, 
			PointerType ControlRegister2Address>
		volatile DcoControlRegister1* const DcoControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address, 
													ControlRegister2Address> 
			::_controlRegister1Addr = reinterpret_cast<volatile DcoControlRegister1*>(ControlRegister1Address);


		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address, 
			PointerType ControlRegister2Address>
		volatile DcoControlRegister2* const DcoControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address, 
													ControlRegister2Address> 
			::_controlRegister2Addr = reinterpret_cast<volatile DcoControlRegister2*>(ControlRegister2Address);


		//
		// FllControl
		//
		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address, 
			PointerType ControlRegister2Address>
		volatile FllControlRegister1* const FllControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address, 
													ControlRegister2Address> 
			::_controlRegister1Addr = reinterpret_cast<volatile FllControlRegister1*>(ControlRegister1Address);


		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address, 
			PointerType ControlRegister2Address>
		volatile FllControlRegister2* const FllControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address, 
													ControlRegister2Address> 
			::_controlRegister2Addr = reinterpret_cast<volatile FllControlRegister2*>(ControlRegister2Address);

		//
		// ClockSourceBindControl
		//
		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address, 
			PointerType ControlRegister2Address>
		volatile ClockSourceSelectRegister* const ClockSourceBindControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address, 
													ControlRegister2Address> 
			::_bindControlRegisterAddr = reinterpret_cast<volatile ClockSourceSelectRegister*>(ControlRegister1Address);

		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address, 
			PointerType ControlRegister2Address>
		volatile ClockDividerRegister* const ClockSourceBindControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address, 
													ControlRegister2Address> 
			::_dividerControlRegisterAddr = reinterpret_cast<volatile ClockDividerRegister*>(ControlRegister2Address);

		//
		// GeneralSettingsControl
		//
		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address>
		volatile GeneralSettingsRegister* const GeneralSettingsControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address>
			::_generalSettingsRegisterAddr = reinterpret_cast<volatile GeneralSettingsRegister*>(ControlRegister1Address);


		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegister1Address>
		volatile ClockSourcFaultFlagsRegister* const ClockSourceFaultFlagsControl<RegisterDataType, 
													PointerType, 
													ControlRegister1Address> 
			::_faultFlagsRegisterAddr = reinterpret_cast<volatile ClockSourcFaultFlagsRegister*>(ControlRegister1Address);

	}
}
}

#endif // CLOCK_SYSTEM_CONTROL_H
