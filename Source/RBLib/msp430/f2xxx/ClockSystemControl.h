
#ifndef CLOCK_SYSTEM_CONTROL_MSP430X2XX_H
#define CLOCK_SYSTEM_CONTROL_MSP430X2XX_H

#include "../x1xxx,x2xxx/ClockSystemControl.h"
#include "ClockSystemRegisters.h"

namespace Rblib
{
namespace Msp430x2xx
{
	namespace ClockSystemParts
	{
		/// ����� ���������� ��������� BCSCTL3.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegisterAddress ����� �������� BCSCTL3.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegisterAddress>
		class Control3
		{
		protected:

			/// ��������� ��������� BCSCTL3.
			static volatile ControlRegister3* const _controlRegisterAddr;

		public:

			/// �������� ��������.
			enum Capasitor
			{
				/// 1��.
				Capasitor1Pf = 0,
				/// 6��.
				Capasitor6Pf = 1,
				/// 10��.
				Capasitor10Pf = 2,
				/// 12.5��.
				Capasitor12Pf = 3
			};

			/// ������ ������ ��� XT1 � XT2.
			enum ExternlaSourceMode
			{
				/// ������� 0.4 - 1 MHz � ������ HF (XT1/XT2) ��� 32768Hz � ������ LF (XT1).
				ExternalSourceMode1 = 0,
				/// ��� ������ 1 - 3 MHz � ������� HF (XT2).
				ExternalSourceMode2 = 1,
				/// ��� ������ 3 - 16 MHz � ������ HF (XT1/XT2) ��� VLOCLK � ������ LF(XT1).
				ExternalSourceMode3 = 2,
				/// ������� �������� ������ �� ���� ��� ����� ������� (HF � LF) ����� ���������� (XT1/XT2).
				ExternalSourceMode4 = 3,
			};

		public:

			/// ������ � �������� BCSCTL3.
			static inline RegisterDataType& GetControlRegister3()
			{
				return (RegisterDataType&) *_controlRegisterAddr;
			}

			/// ������ �������� ������� ��� XT1 � ������ ������ ������� (XCAPx).
			static inline void SetExternalClock1Capasitor(Capasitor value)
			{
				_controlRegisterAddr->ExternalSource1Capacitor = value;
			}

			/// ���������� ����� ������ XT1 (LFXT1Sx).
			static inline void SetExternalClock1Mode(ExternlaSourceMode value)
			{
				_controlRegisterAddr->ExternalSource1Settings = value;
			}

			/// ���������� ����� ������ ����� ������ XT1 (LFXT1Sx).
			static inline ExternlaSourceMode GetExternalClock1Mode()
			{
				return (ExternlaSourceMode) _controlRegisterAddr->ExternalSource1Settings;
			}

			/// ���������� ����� ������ XT2 (XT2Sx).
			static inline void SetExternalClock2Mode(ExternlaSourceMode value)
			{
				_controlRegisterAddr->ExternalSource2Settings = value;
			}

			/// ���������� ����� ������ ����� ������ XT2 (XT2Sx).
			static inline ExternlaSourceMode GetExternalClock2Mode()
			{
				return (ExternlaSourceMode) _controlRegisterAddr->ExternalSource2Settings;
			}

			/// ��������� ������� ������ ����������� XT1 (LFXT1OF).
			/// \return @c true, ���� ������ ����, ��� @c false, ���� ������ ���.
			static inline bool GetExternalClock1Fault()
			{
				return _controlRegisterAddr->ExternalSource1Fault;
			}

			/// ��������� ������� ������ ����������� XT2 (XT2OF).
			/// \return @c true, ���� ������ ����, ��� @c false, ���� ������ ���.
			static inline bool GetExternalClock2Fault()
			{
				return _controlRegisterAddr->ExternalSource2Fault;
			}

			/// �������� ���� ������ ����������� XT1 (LFXT1OF).
			static inline void ClearExternalClock1Fault()
			{
				_controlRegisterAddr->ExternalSource1Fault = 0;
			}

			/// �������� ���� ������ ����������� XT2 (XT2OF).
			static inline void ClearExternalClock2Fault()
			{
				_controlRegisterAddr->ExternalSource2Fault = 0;
			}

			/// ��������� ������� ������ ������������ XT1 � XT2 (LFXT1OF,XT2OF).
			/// \return @c true, ���� ������ ����, ��� @c false, ���� ������ ���.
			static inline bool GetExternalClockFaults()
			{
				return _controlRegisterAddr->ExternalSourceFaults;
			}

			/// �������� ����� ������ (LFXT1OF,XT2OF).
			static inline void ClearExternalClockFaults()
			{
				_controlRegisterAddr->ExternalSourceFaults = 0;
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
				PointerType ControlRegister2Address, 
				PointerType ControlRegister3Address>
    class ClockSourceBindControl : public Msp430x12xxx::ClockSystemParts::ClockSourceBindControl<RegisterDataType, PointerType, ControlRegister1Address, ControlRegister2Address>
		{
		protected:

      typedef Msp430x12xxx::ClockSystemParts::ClockSourceBindControl<RegisterDataType, PointerType, ControlRegister1Address, ControlRegister2Address> _Base;
			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		public:

			/// ��������� ��������� �������.
			enum ClockSource
			{
        /// XT1
        ClockSourceExternal1 = _Base::ClockSourceExternal1, // XT1
        /// XT2
        ClockSourceExternal2 = _Base::ClockSourceExternal2, // XT2
        /// DCO
        ClockSourceInternal = _Base::ClockSourceInternal, // DCO
				ClockSourceInternalLowFreq1 = 4 // VLOCLK
			};

		public:

			/// ������ � �������� BCSCTL3.
			static inline RegisterDataType& GetControlRegister3()
			{
				return _Control3::GetControlRegister3();
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
					// ���������, �� ��������� �� XT1 � ������ VLO
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
						return false;
					_Base::_Control2::SetCoreSignalSource(_Base::_Control2::CoreSignalSourceExternal1);
					return true;
				case ClockSourceInternalLowFreq1:
					// VLO � ������ ��������� - ����� XT1
					// ���������, � ������ �� ������ XT1
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
					{
						_Base::_Control2::SetCoreSignalSource(_Base::_Control2::CoreSignalSourceExternal1);
						return true;
					}
				}

        return _Base::SetCoreSignalSource((typename _Base::ClockSource)source);
			}

      /// ������ �������� SMCLK.
      /// \param source �������� �������� �������.
      /// return @c true - ��������� �������. @c false - �� ������� ��������� �������� (������� ��������� �� ��������� ������������ �������� ��������, ���� SMCLK �� ����� �������� � �������� ����������).
      static inline bool SetPeripherySignalSource(ClockSource source)
      {
        return _Base::SetPeripherySignalSource((typename _Base::ClockSource)source);
      }

			/// ������ �������� ACLK.
			/// \param source �������� �������� �������.
			/// return @c true - ��������� �������. @c false - �� ������� ��������� �������� (������� ��������� �� ��������� ������������ �������� ��������, ���� ACLK �� ����� �������� � �������� ����������).
			static inline bool SetPeripheryAuxSignalSource(ClockSource source)
			{
				switch(source)
				{
				case ClockSourceExternal1:
					// ���������, �� ��������� �� XT1 � ������ VLO
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
						return false;
					return true;
				case ClockSourceInternalLowFreq1:
					// VLO � ������ ��������� - ����� XT1
					// ���������, � ������ �� ������ XT1
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
						return true;
					return false;
				}
        return false;
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
    class DcoControl : public Msp430x12xxx::ClockSystemParts::DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address>
		{
        protected:
            typedef Msp430x12xxx::ClockSystemParts::DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address> _Base;
		public:

			/// ���������� ������������ �������� ��������� ������� (���� RSELx).
			static inline unsigned char MaxFrequencyRange()
			{
				if (_Base::ResistorExternal())
					return 0x07;
				else
					return 0x0F;
			}

		};

		/// ����� ���������� ������� ������ ������������ (BCSCTL3).
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegisterAddress3 ����� �������� BCSCTL3.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegister3Address>
		class ClockSourceFaultFlagsControl
		{
		protected:

			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		public:

			/// ������ � �������� BCSCTL3.
			static inline RegisterDataType& GetControlRegister3()
			{
				return _Control3::GetControlRegister3();
			}

			/// ��������� ������� ������ ��������� XT1 (LFXT1OF).
			/// \return @c true - ���� ������. @c false - ��� ������.
			static inline bool ExternalSource1Fault()
			{
				return _Control3::GetExternalClock1Fault();
			}

			/// ��������� ������� ������ ��������� XT2 (XT2OF).
			/// \return @c true - ���� ������. @c false - ��� ������.
			static inline bool ExternalSource2Fault()
			{
				return _Control3::GetExternalClock2Fault();
			}

			/// ���������� ����� ������ �������� XT1 (LFXT1OF).
			static inline void ClearExternalSource1Fault()
			{
				_Control3::ClearExternalClock1Fault();
			}

			/// ���������� ����� ������ �������� XT2 (XT2OF).
			static inline void ClearExternalSource2Fault()
			{
				_Control3::ClearExternalClock2Fault();
			}

			/// ��������� ������� ������ ���������� XT1/XT2 (LFXT1OF, XT2OF).
			/// \return @c true - ���� ������. @c false - ��� ������.
			static inline bool HasFaults()
			{
				return _Control3::GetExternalClockFaults();
			}
			
			/// �������� ����� ������ ������������ (LFXT1OF, XT2OF).
			static inline void ClearSourceFaults()
			{
				_Control3::ClearExternalClockFaults();
			}

			/// ���������� �������� ������������ ���� LFXT1OF + XT2OF
			static inline RegisterDataType GetSourceFaults()
			{
				return _Control3::GetExternalClockFaults();
			}
		};

		//
		// Control3
		//
		template<class RegisterDataType, 
			class PointerType, 
			PointerType ControlRegisterAddress>
		volatile ControlRegister3* const Control3<RegisterDataType, 
													PointerType, 
													ControlRegisterAddress>
			::_controlRegisterAddr = reinterpret_cast<volatile ControlRegister3*>(ControlRegisterAddress);
	}
}
}

#endif // CLOCK_SYSTEM_CONTROL_MSP430X2XX_H
