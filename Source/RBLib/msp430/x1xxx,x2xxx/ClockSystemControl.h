
#ifndef CLOCK_SYSTEM_CONTROL_MSP430X1XXX_MSP430X2XXX_H
#define CLOCK_SYSTEM_CONTROL_MSP430X1XXX_MSP430X2XXX_H

#include "ClockSystemRegisters.h"

namespace Rblib
{
namespace Msp430x12xxx
{
	namespace ClockSystemParts
	{

		/// ����� ��� ������ � ������� ��������-���������
		class SourceDivider
		{
		public:

			/// ��������.
			enum Divider
			{
				/// 1
				Divider1 = 0, // 1
				/// 2
				Divider2 = 1, // 2
				/// 4
				Divider4 = 2, // 4
				/// 8
				Divider8 = 3  // 8
			};

			/// ������������� �������� �� ����� � ����, �������� �� ��������.
			/// \param divider ��������-�����.
			/// \return ��������-����.
			static inline Divider GetDivider(unsigned char divider)
			{
				if (divider >= 8)
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

			/// ������������� �������� �� ����� � ����, �������� �� ��������.
			/// \param divider ��������-�����.
			/// \return ��������-����.
			static inline Divider GetUpDivider(unsigned char divider)
			{
				if (divider > 4)
					return Divider8;
				else if (divider > 2)
					return Divider4;
				else if (divider > 1)
					return Divider2;
				else
					return Divider1;
			}

			/// ������������� �������� �� ����� � �����.
			/// \param divider ��������-����.
			/// \return ��������-�����.
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
				}
				return 1;
			}

		};

		/// ����� ���������� ��������� BCSCTL1.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegisterAddress ����� �������� BCSCTL1.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegisterAddress>
		class Control1 : virtual public SourceDivider
		{
		protected:

			/// ��������� �� ������� BCSCTL1.
			static volatile ControlRegister1* const _controlRegisterAddr;

		public:

			/// ������ � �������� BCSCTL1.
			static inline RegisterDataType& GetControlRegister1()
			{
				return (RegisterDataType&) *_controlRegisterAddr;
			}

			/// ������ �������� ������� (RSELx).
			/// \param value �������� ������� 0..7, ���� ������������ ������� �������� (DCOR=1), ��� 0..15, ���� ������������ ���������� �������� (DCOR=0).
			static inline void SetInternalClockFrequencyRange(RegisterDataType value)
			{
				// ��������� ��� ���������� (slaz033b.pdf, BCL12)
				if (_controlRegisterAddr->FrequencyRange > 13 && value < 13)
				{
					_controlRegisterAddr->FrequencyRange = 13;
				}
				_controlRegisterAddr->FrequencyRange = value;
			}

			/// ���������� ��������� �������� ������ (RSELx).
			static inline RegisterDataType GetInternalClockFrequencyRange()
			{
				return _controlRegisterAddr->FrequencyRange;
			}

			/// ��������� �� 1 �������� ���� RSELx.
			static inline void IncInternalClockFrequencyRange()
			{
				_controlRegisterAddr->FrequencyRange++;
			}

			/// ��������� �� 1 �������� ���� RSELx.
			static inline void DecInternalClockFrequencyRange()
			{
				_controlRegisterAddr->FrequencyRange--;
			}

			/// ������ �������� ACLK (DIVAx).
			static inline void SetPeripheryAuxSignalDivider(Divider value)
			{
				_controlRegisterAddr->PeripheryAuxClockDivider = value;
			}

			/// ���������� ������� ������� ������ XT1 (XTS).
			/// \param highFreqMode �������� ���� XTS.
			static inline void SetExternalClock1HighFreqMode(bool highFreqMode)
			{
				_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable = highFreqMode;
			}

			/// �������� ����� ������� ������� XT1 (XTS=1).
			static inline void ExternalClock1HighFreqModeEnable()
			{
				_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable = 1;
			}

			/// ��������� ����� ������� ������� XT1 (XTS=0).
			static inline void ExternalClock1HighFreqModeDisable()
			{
				_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable = 0;
			}

			/// ��������� ������� �� ����� ������� ������ XT1.
			/// return @c true, ���� ����� �� �������, ��� @c false, ���� ����� �� ���������.
			static inline bool ExternalClock1HighFreqModeEnabled()
			{
				return _controlRegisterAddr->ExternalSource1HighFrequencyModeEnable;
			}

			/// ��������� �������� �� ����� ������� ������ XT1.
			/// return @c true, ���� ����� �� ��������, ��� @c false, ���� ����� �� ��������.
			static inline bool ExternalClock1HighFreqModeDisabled()
			{
				return (_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable == 0);
			}

			/// �������� XT2.
			static inline void ExternalClock2Enable()
			{
				_controlRegisterAddr->ExternalSource2Off = 0;
			}

			/// ��������� XT2.
			/// \details ��������� XT2, ���� �� �� ������������ � �������� �������� ��� MCLK ��� SMCLK.
			static inline void ExternalClock2Disable()
			{
				_controlRegisterAddr->ExternalSource2Off = 1;
			}

		};

		/// ����� ���������� ��������� BCSCTL2.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegisterAddress ����� �������� BCSCTL2.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegisterAddress>
		class Control2 : virtual public SourceDivider
		{
		protected:

			/// ��������� �� ������� BCSCTL2.
			static volatile ControlRegister2* const _controlRegisterAddr;

		public:

			/// ��������� ��� MCLK.
			enum CoreSignalSource
			{
				/// DCO.
				CoreSignalSourceInternal = 0x00,
				/// XT1.
				CoreSignalSourceExternal1 = 0x11,
				/// XT2.
				CoreSignalSourceExternal2 = 0x10
			};

			/// ��������� ��� MCLK.
			enum PeripherySignalSource
			{
				/// DCO.
				PeripherySignalSourceInternal = 0,
				/// XT2, ���� �� ����, XT1/VLOCLK, ���� ���� XT2.
				PeripherySignalSourceExternal = 1
			};

		public:

			/// ������ � �������� BCSCTL2.
			static inline RegisterDataType& GetControlRegister2()
			{
				return (RegisterDataType&) *_controlRegisterAddr;
			}

			/// ���������� �������� ��� SMCLK (DIVSx).
			static inline void SetPeripherySignalDivider(Divider value)
			{
				_controlRegisterAddr->PeripheryClockDivider = value;
			}

			/// ���������� �������� ��� MCLK (DIVMx).
			static inline void SetCoreSignalDivider(Divider value)
			{
				_controlRegisterAddr->CoreClockDivider = value;
			}

			/// ������ �������� ��� SMCLK (SELS).
			static inline void SetPeripherySignalSource(PeripherySignalSource value)
			{
				_controlRegisterAddr->PeripheryClockSource = value;
			}

			/// ������ �������� ��� MCLK (SELMx).
			static inline void SetCoreSignalSource(CoreSignalSource value)
			{
				_controlRegisterAddr->CoreClockSource = value;
			}

			/// ������������ ���������� �������� (DCOR=0).
			static inline void SetInternalSourceResistorInternal()
			{
				_controlRegisterAddr->InternalSourceResistor = 0;
			}

			/// ������������ ������� �������� (DCOR=1).
			static inline void SetInternalSourceResistorExternal()
			{
				_controlRegisterAddr->InternalSourceResistor = 1;
			}

			/// ��������� ����� �������� ������������.
			/// return @c true, ���� ������������ ������� �������� (DCOR=1), ��� @c false, ���� ������������ ���������� �������� (DCOR=0).
			static inline bool InternalSourceResistorExternal()
			{
				return _controlRegisterAddr->InternalSourceResistor;
			}

		};

		/// ���������� ����� ��� ���������� ��������� ���������� � �������� � ����������.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegisterAddress1 ����� �������� BCSCTL1.
		/// \tparam ControlRegisterAddress2 ����� �������� BCSCTL2.
		/// \tparam ControlRegisterAddress3 ����� �������� BCSCTL3.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegister1Address,
				PointerType ControlRegister2Address>
		class ClockSourceBindControl : virtual public SourceDivider
		{
		protected:

			typedef Control1<RegisterDataType, PointerType, ControlRegister1Address> _Control1;
			typedef Control2<RegisterDataType, PointerType, ControlRegister2Address> _Control2;

		public:

			/// ��������� ��������� �������.
			enum ClockSource
			{
				/// XT1
				ClockSourceExternal1 = 1, // XT1
				/// XT2
				ClockSourceExternal2 = 2, // XT2
				/// DCO
				ClockSourceInternal = 3, // DCO
			};

		public:

			/// ������ � �������� BCSCTL1.
			static inline RegisterDataType& GetControlRegister1()
			{
				return _Control1::GetControlRegister1();
			}

			/// ������ � �������� BCSCTL2.
			static inline RegisterDataType& GetControlRegister2()
			{
				return _Control2::GetControlRegister2();
			}

			// ���������

			/// ������ �������� MCLK.
			/// \param source �������� �������� �������.
			/// return @c true - ��������� �������. @c false - �� ������� ��������� �������� (������� ��������� �� ��������� ������������ �������� ��������).
			static inline bool SetCoreSignalSource(ClockSource source)
			{
				switch(source)
				{
				case ClockSourceExternal1:
					_Control2::SetCoreSignalSource(_Control2::CoreSignalSourceExternal1);
					return true;
				case ClockSourceExternal2:
					_Control2::SetCoreSignalSource(_Control2::CoreSignalSourceExternal2);
					return true;
				case ClockSourceInternal:
					_Control2::SetCoreSignalSource(_Control2::CoreSignalSourceInternal);
					return true;
				}

				return false;
			}

			/// ������ �������� SMCLK.
			/// \param source �������� �������� �������.
			/// return @c true - ��������� �������. @c false - �� ������� ��������� �������� (������� ��������� �� ��������� ������������ �������� ��������, ���� SMCLK �� ����� �������� � �������� ����������).
			static inline bool SetPeripherySignalSource(ClockSource source)
			{
				switch(source)
				{
				case ClockSourceExternal2:
					_Control2::SetPeripherySignalSource(_Control2::PeripherySignalSourceExternal);
					return true;
				case ClockSourceInternal:
					_Control2::SetPeripherySignalSource(_Control2::PeripherySignalSourceInternal);
					return true;
				}
				return false;
			}

			/// ������ �������� ACLK.
			/// \param source �������� �������� �������.
			/// return @c true - ��������� �������. @c false - �� ������� ��������� �������� (������� ��������� �� ��������� ������������ �������� ��������, ���� ACLK �� ����� �������� � �������� ����������).
			static inline bool SetPeripheryAuxSignalSource(ClockSource source)
			{
				switch(source)
				{
				case ClockSourceExternal1:
					return true;
				}

				return false;
			}

			// ��������

			/// ���������� �������� MCLK (DIVMx/BCSCTL2).
			static inline void SetCoreSignalDivider(Divider divider)
			{
				_Control2::SetCoreSignalDivider(divider);
			}

			/// ���������� �������� SMCLK (DIVSx)/BCSCTL2.
			static inline void SetPeripherySignalDivider(Divider divider)
			{
				_Control2::SetPeripherySignalDivider(divider);
			}

			/// ���������� �������� ACLK (DIVAx/BCSCTL1).
			static inline void SetPeripheryAuxSignalDivider(Divider divider)
			{
				_Control1::SetPeripheryAuxSignalDivider(divider);
			}

			/// ���������� �������� MCLK (DIVMx/BCSCTL2).
			static inline void SetCoreSignalDivider(unsigned long divider)
			{
				_Control2::SetCoreSignalDivider(GetDivider(divider));
			}

			/// ���������� �������� SMCLK (DIVSx/BCSCTL2).
			static inline void SetPeripherySignalDivider(unsigned long divider)
			{
				_Control2::SetPeripherySignalDivider(GetDivider(divider));
			}

			/// ���������� �������� ACLK (DIVAx/BCSCTL1).
			static inline void SetPeripheryAuxSignalDivider(unsigned long divider)
			{
				_Control1::SetPeripheryAuxSignalDivider(GetDivider(divider));
			}
		};

		/// ����� ���������� ��������� DCOCTL 
		/// � ����������� DCO �� ������ ����������� ��������� (BCSCTL1 � BCSCTL2).
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam DcoControlRegisterAddress ����� �������� DCOCTL.
		/// \tparam ControlRegisterAddress1 ����� �������� BCSCTL1.
		/// \tparam ControlRegisterAddress2 ����� �������� BCSCTL2.
		template<class RegisterDataType,
				class PointerType,
				PointerType DcoControlRegisterAddress,
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class DcoControl
		{

		protected:

			/// ��������� �� ������� DCOCTL
			static volatile DcoControlRegister* const _dcoControlRegisterAddr;

		protected:

			typedef Control1<RegisterDataType, PointerType, ControlRegister1Address> _Control1;
			typedef Control2<RegisterDataType, PointerType, ControlRegister2Address> _Control2;

		public:

			/// ���������.
			enum Constants
			{
				/// ������������ �������� ������������ ���� DCO + MOD �������� DCOCTL.
				MaxUnifiedTapSelect = 0xFF,
			};

		public:

			/// ������ � �������� DCOCTL.
			static inline RegisterDataType& GetDcoControlRegister()
			{
				return (RegisterDataType&) *_dcoControlRegisterAddr;
			}

			/// ������ � �������� BCSCTL1.
			static inline RegisterDataType& GetControlRegister1()
			{
				return _Control1::GetControlRegister1();
			}

			/// ������ � �������� BCSCTL2.
			static inline RegisterDataType& GetControlRegister2()
			{
				return _Control2::GetControlRegister2();
			}

			static inline void EnableModulation()
			{
			}

			/// ������������� �������� ���� MOD �������� DCOCTL.
			static inline void SetModulationCounter(RegisterDataType modulatorSelect)
			{
				_dcoControlRegisterAddr->ModulatorSelect = modulatorSelect;
			}

			/// ���������� �������� ���� MOD �������� DCOCTL.
			static inline RegisterDataType GetModulationCounter()
			{
				return _dcoControlRegisterAddr->ModulatorSelect;
			}

			/// ������������� �������� ���� DCO �������� DCOCTL.
			static inline void SetTapSelect(RegisterDataType frequencySelect)
			{
				_dcoControlRegisterAddr->FrequencySelect = frequencySelect;
			}

			/// ���������� �������� ���� DCO �������� DCOCTL.
			static inline RegisterDataType GetTapSelect()
			{
				return _dcoControlRegisterAddr->FrequencySelect;
			}

			/// ������������� �������� ������������ ���� DCO + MOD �������� DCOCTL.
			static inline void SetUnifiedTapSelect(RegisterDataType tapField)
			{
				_dcoControlRegisterAddr->UnifiedTapField = tapField;
			}

			/// ���������� �������� ������������ ���� DCO + MOD �������� DCOCTL.
			static inline RegisterDataType GetUnifiedTapSelect()
			{
				return _dcoControlRegisterAddr->UnifiedTapField;
			}

			/// ����������� �� 1 �������� ������������ ���� DCO + MOD �������� DCOCTL.
			static inline void IncUnifiedTapSelect()
			{
				_dcoControlRegisterAddr->UnifiedTapField++;
			}

			/// ��������� �� 1 �������� ������������ ���� DCO + MOD �������� DCOCTL.
			static inline void DecUnifiedTapSelect()
			{
				_dcoControlRegisterAddr->UnifiedTapField--;
			}

			/// ���������� �������� �������. ���� RSELx �������� BCSCTL1.
			static inline RegisterDataType GetFrequencyRange()
			{
				return _Control1::GetInternalClockFrequencyRange();
			}

			/// ������������� �������� �������. ���� RSELx �������� BCSCTL1.
			static inline void SetFrequencyRange(RegisterDataType frequencyRange)
			{
				_Control1::SetInternalClockFrequencyRange(frequencyRange);
			}

			/// ����������� �� 1 �������� ���� RSELx �������� BCSCTL1.
			static inline void IncFrequencyRange()
			{
				_Control1::IncInternalClockFrequencyRange();
			}

			/// ��������� �� 1 �������� ���� RSELx �������� BCSCTL1.
			static inline void DecFrequencyRange()
			{
				_Control1::DecInternalClockFrequencyRange();
			}

			/// ������������ ���������� ��������.
			static inline void SetResistorInternal()
			{
				_Control2::SetInternalSourceResistorInternal();
			}

			/// ������������ ������� ��������.
			static inline void SetResistorExternal()
			{
				_Control2::SetInternalSourceResistorExternal();
			}

			/// ��������� ������������ �� ������� ��������.
			/// return @c true - ������� ��������. @c false - ���������� ��������.
			static inline bool ResistorExternal()
			{
				return _Control2::InternalSourceResistorExternal();
			}

		};

		//
		// ��������������� ����������� ����������
		//
		// DcoControl
		//
		template<class RegisterDataType, 
				class PointerType, 
				PointerType DcoControlRegisterAddress,
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		volatile DcoControlRegister* const DcoControl<RegisterDataType, 
													PointerType, 
													DcoControlRegisterAddress,
													ControlRegister1Address,
													ControlRegister2Address> 
			::_dcoControlRegisterAddr = reinterpret_cast<volatile DcoControlRegister*>(DcoControlRegisterAddress);

		
		//
		// Control1
		//
		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegisterAddress>
		volatile ControlRegister1* const Control1<RegisterDataType, 
													PointerType, 
													ControlRegisterAddress>
			::_controlRegisterAddr = reinterpret_cast<volatile ControlRegister1*>(ControlRegisterAddress);


		//
		// Control2
		//
		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegisterAddress>
		volatile ControlRegister2* const Control2<RegisterDataType, 
													PointerType, 
													ControlRegisterAddress>
			::_controlRegisterAddr = reinterpret_cast<volatile ControlRegister2*>(ControlRegisterAddress);

	}
}
}

#endif // CLOCK_SYSTEM_CONTROL_MSP430X1XXX_MSP430X2XXX_H
