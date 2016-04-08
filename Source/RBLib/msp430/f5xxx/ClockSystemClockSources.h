/////////////////////////////////////////
//
// ������ ���������� ��������� �������
//
/////////////////////////////////////////

#ifndef CLOCK_SYSTEM_CLOCK_SOURCES_H
#define CLOCK_SYSTEM_CLOCK_SOURCES_H

#include <math.h>

#include "ClockSystemControl.h"
#include "../../Timer.h"
#include "../FrequencyCalibration.h"

namespace Rblib
{
namespace Msp430f5xxx
{
	namespace ClockSystemParts
	{

		/// \brief ������� ������� XT1.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� ��������.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ExternalClockSource1Impl
		{
		protected:
			
			typedef GeneralSettingsControl<RegisterDataType, PointerType, ControlRegister1Address> SettingsControl;

		public:

			/// ������������ ���������� ������������ ���.
			/// \details �� ���������� ������ ������� ��������� ������ ���������.
			enum CurrentAdjust
			{
				/// ����� ������ ����������������� � ������ LF. ���������� ������� � ������ HF 4-8 Mhz.
				CurrentAdjustLevel1 = SettingsControl::CurrentAdjustLevel1,
				/// ���������� ��������� � ������ LF. ���������� ������� � ������ HF 8-16 Mhz.
				CurrentAdjustLevel2 = SettingsControl::CurrentAdjustLevel2,
				/// ���������� ��������� � ������ LF. ���������� ������� � ������ HF 16-24 Mhz.
				CurrentAdjustLevel3 = SettingsControl::CurrentAdjustLevel3,
				/// ������������ ��������� � ������������ ���� � ������ LF. ���������� ������� � ������ HF 24-32 Mhz.
				CurrentAdjustLevel4 = SettingsControl::CurrentAdjustLevel4
			};

			/// ��������� �������� ��������.
			enum Capasitor
			{
				/// 2��
				Capasitor2Pf = SettingsControl::Capasitor2Pf,
				/// 6��
				Capasitor6Pf = SettingsControl::Capasitor6Pf,
				/// 9��
				Capasitor9Pf = SettingsControl::Capasitor9Pf,
				/// 12��
				Capasitor12Pf = SettingsControl::Capasitor12Pf
			};

		public:

			/// �������� ��������.
			static inline void On()
			{
				SettingsControl::ExternalClock1On();
			}

			/// ��������� ��������.
			static inline void Off()
			{
				SettingsControl::ExternalClock1Off();
			}

			/// ���������� ������������ ������������ ���.
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				SettingsControl::SetExternalClock1CurrentAdjust((typename SettingsControl::CurrentAdjust)currentAdjust);
			}

			/// �������� ����� ������������ ���������������� ��������� �������.
			static inline void HighFreqModeEnable()
			{
				SettingsControl::ExternalClock1HighFreqModeEnable();
			}

			/// ��������� ����� ������������ ���������������� ��������� �������.
			static inline void HighFreqModeDisable()
			{
				SettingsControl::ExternalClock1HighFreqModeDisable();
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT1.
			static inline void PinModeEnable()
			{
				SettingsControl::ExternalClock1PinModeEnable();
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT1.
			static inline void PinModeDisable()
			{
				SettingsControl::ExternalClock1PinModeDisable();
			}

			/// ������� �������.
			static inline void SetCapasitor(Capasitor capasitor)
			{
				SettingsControl::SetExternalClock1Capasitor((typename SettingsControl::Capasitor)capasitor);
			}

		};

		/// \brief ������� ������� XT2.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� ��������.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ExternalClockSource2Impl
		{
		protected:
			
			typedef GeneralSettingsControl<RegisterDataType, PointerType, ControlRegister1Address> SettingsControl;

		public:

			/// ������������ ���������� ������������ ���.
			/// \details �� ���������� ������ ������� ��������� ������ ���������.
			enum CurrentAdjust
			{
				/// ����� ������ ����������������� � ������ LF. ���������� ������� � ������ HF 4-8 Mhz.
				CurrentAdjustLevel1 = SettingsControl::CurrentAdjustLevel1,
				/// ���������� ��������� � ������ LF. ���������� ������� � ������ HF 8-16 Mhz.
				CurrentAdjustLevel2 = SettingsControl::CurrentAdjustLevel2,
				/// ���������� ��������� � ������ LF. ���������� ������� � ������ HF 16-24 Mhz.
				CurrentAdjustLevel3 = SettingsControl::CurrentAdjustLevel3,
				/// ������������ ��������� � ������������ ���� � ������ LF. ���������� ������� � ������ HF 24-32 Mhz.
				CurrentAdjustLevel4 = SettingsControl::CurrentAdjustLevel4
			};

		public:

			/// �������� ��������.
			static inline void On()
			{
				SettingsControl::ExternalClock2On();
			}

			/// ��������� ��������.
			static inline void Off()
			{
				SettingsControl::ExternalClock2Off();
			}

			/// ���������� ������������ ������������ ���.
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				SettingsControl::SetExternalClock2CurrentAdjust((typename SettingsControl::CurrentAdjust)currentAdjust);
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT2.
			static inline void PinModeEnable()
			{
				SettingsControl::ExternalClock2PinModeEnable();
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT2.
			static inline void PinModeDisable()
			{
				SettingsControl::ExternalClock2PinModeDisable();
			}
		};

		/// ���������� �������� DCO.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType DcoControlRegister1Address,
				PointerType DcoControlRegister2Address,
				PointerType FllControlRegister1Address,
				PointerType FllControlRegister2Address,
				PointerType BindControlRegister1Address,
				PointerType BindControlRegister2Address,
				class FrequencyRangeSelect,
				class CalibrationParams>
		class InternalClockSourceImpl
			: public DcoControl<RegisterDataType, PointerType, DcoControlRegister1Address, DcoControlRegister2Address>
			, public FllControl<RegisterDataType, PointerType, FllControlRegister1Address, FllControlRegister2Address>
		{
		protected:

			class Regulator;

			typedef DcoControl<RegisterDataType, PointerType, DcoControlRegister1Address, DcoControlRegister2Address> _DcoControl;
			typedef FllControl<RegisterDataType, PointerType, FllControlRegister1Address, FllControlRegister2Address> _FllControl;
			typedef ClockSourceBindControl<RegisterDataType, PointerType, BindControlRegister1Address, BindControlRegister2Address> _BindControl;
			typedef Msp430::ClockSystemParts::FrequencyCalibration<_BindControl, CalibrationParams, Regulator> _FrequencyCalibration;

		public:

			/// ��������������� ������� ������� ���������.
			enum StandardFrequency
			{
				/// 1MHz.
				Frequency1MHz = 1048576,
				/// 2MHz.
				Frequency2MHz = 2097152,
				/// 4MHz.
				Frequency4MHz = 4194304,
				/// 8MHz.
				Frequency8MHz = 8388608,
				/// 16MHz.
				Frequency16MHz = 16777216,
				/// 20MHz.
				Frequency20MHz = 20000000,
				/// 25MHz.
				Frequency25MHz = 25000000,
				/// 30MHz.
				Frequency30MHz = 30000000,
                /// 33MHz
                Frequency33MHz = 33554432,
				/// ������������ ������� (33MHz).
				FrequencyMax = Frequency33MHz,
				/// ������������ ������� ���� (25MHz).
				FrequencyMaxCore = Frequency25MHz,
				/// ������� �� ��������� (16MHz).                  
                FrequencyDefault = Frequency16MHz
			};

		public:

			/// �������� FLL.
			static inline void EnableFll()
			{
				OperatingMode::EnableFll();
			}

			/// ��������� FLL.
			static inline void DisableFll()
			{
				OperatingMode::DisableFll();
			}

			/// ��������� ������� �� FLL.
			static inline bool IsFllOn()
			{
				return OperatingMode::IsFllEnabled();
			}

			/// ���������� ������� ���������, ��������� ����������� ����������� FLL.
			/// \param referenceSource ������� ���������� ��������� ������� ��� FLL.
			/// \param sourceFreq ������� ���������� �������.
			/// \param requiredFreq �������, �� ������� ���� ��������� ��������.
			/// \param fllDivider �������� ��������� ������� FLL.
			static unsigned long SetFrequencyFll(typename _FllControl::FllReferenceSelect referenceSource,
   														unsigned long sourceFreq,
   														unsigned long requiredFreq,
														typename _FllControl::FllDivider fllDivider = _FllControl::FllDivider1)
			{
				if (requiredFreq > FrequencyMax)
					requiredFreq = FrequencyMax;

				bool startFll = false;
				if (IsFllOn())
				{
					startFll = true;
					DisableFll();
				}
				
				_DcoControl::SetUnifiedTapSelect(0);

				_FllControl::SetFllReference(referenceSource);
				_FllControl::SetFllDivider(fllDivider);

				unsigned char fllDividerValue = _FllControl::fllDividerToValue(fllDivider);
				double generalMultipler = (double)requiredFreq / (double)sourceFreq / (double)fllDividerValue;

				typename _FllControl::FllReferenceDivider refDividerEnum = _FllControl::valueToFllReferenceDivider((unsigned char)((double) _FllControl::FllMaxMultiplierValue / generalMultipler));
				unsigned char refDividerValue = _FllControl::fllReferenceDividerToValue(refDividerEnum);

				_FllControl::SetFllReferenceDivider(refDividerEnum);

				unsigned int resultMultipler = (unsigned int)(generalMultipler * (double) refDividerValue);
				_FllControl::SetFllMultiplier(resultMultipler);

				_DcoControl::SetFrequencyRange(FrequencyRangeSelect::Calculate(requiredFreq));

				if (startFll)
					EnableFll();

				return fllDividerValue * resultMultipler * sourceFreq / refDividerValue;
			}

			/// ���������� ������� ���������, ��������� ��������� ����������.
			/// \details ��� ������� �� ��������� ������� ��������� �������.
			/// ��� ������ ������ �������� ��������� ����� (CorrectFrequency ��� ��������� ������� �������� ����������� �������).
			/// \param referenceClockSource ������� ���������� ��������� �������.
			/// \param referenceSourceFreq ������� ���������� �������.
			/// \param requiredFreq �������, �� ������� ���� ��������� ��������.
			static unsigned long SetFrequency(typename _BindControl::ClockSource referenceClockSource, // �������� ��������� �������
												unsigned long referenceSourceFreq, // �������� ��������� �������
												unsigned long requiredFreq) // ��������� ������� DCO
			{
				if (0 == referenceSourceFreq || 0 == requiredFreq)
					return 0;

				if (requiredFreq > FrequencyMax)
					requiredFreq = FrequencyMax;

				if (IsFllOn())
				{
					DisableFll();
				}

				RegistersState state;
				GetRegistersState(state);

				Regulator::Initialize(requiredFreq); // ���������� ��������� ��������
				unsigned long resultFrequency = _FrequencyCalibration::CorrectFrequency(_BindControl::ClockSourceInternal
																					, referenceClockSource
																					, referenceSourceFreq
																					, requiredFreq);

				SetRegistersState(state);

				return resultFrequency;
			}

			/// ���������� ������� ���������, ��������� ��������� ����������.
			/// \details ��� ������� "�������������" �� ������� ���������.
			/// �������� ������ ��� ��������� �������. ��� ������ ������ � ����� ������ �������� ������, ��� SetFrequency.
			/// \param referenceClockSource ������� ���������� ��������� �������.
			/// \param referenceSourceFreq ������� ���������� �������.
			/// \param requiredFreq �������, �� ������� ���� ��������� ��������.
			static unsigned long CorrectFrequency(typename _BindControl::ClockSource referenceClockSource, // �������� ��������� �������
												unsigned long referenceSourceFreq, // �������� ��������� �������
												unsigned long requiredFreq) // ��������� ������� DCO
			{
				if (0 == referenceSourceFreq || 0 == requiredFreq)
					return 0;

				if (requiredFreq > FrequencyMax)
					requiredFreq = FrequencyMax;

				if (IsFllOn())
				{
					DisableFll();
				}

				RegistersState state;
				GetRegistersState(state);

				// �� ������������� ��������� ��������, �������� � ������� ���������
				unsigned long resultFrequency = _FrequencyCalibration::CorrectFrequency(_BindControl::ClockSourceInternal
																					, referenceClockSource
																					, referenceSourceFreq
																					, requiredFreq);

				SetRegistersState(state);

				return resultFrequency;
			}
			
			/// ���������� ������� ���������.
			/// \param referenceClockSource �������� ��������� �������.
			/// \param referenceSourceFreq ������� ���������� ���������.
			static unsigned long CalcFrequency(typename _BindControl::ClockSource referenceClockSource, // �������� ��������� �������
												unsigned long referenceSourceFreq) // �������� ��������� �������
			{
				if (0 == referenceSourceFreq)
					return 0;

				RegistersState state;
				GetRegistersState(state);

				unsigned long resultFrequency = _FrequencyCalibration::CalcFrequency(_BindControl::ClockSourceInternal
																						, referenceClockSource
																						, referenceSourceFreq);

				SetRegistersState(state);

				return resultFrequency;
			}

		protected:

			/// ��������� ��� �������� ��������� ��������� ������� � ������� ������������ �� ����� ����������.
			struct RegistersState
			{
				typename CalibrationParams::T::RegisterValueType TimerCTLReg;
				typename CalibrationParams::T::RegisterValueType TimerExpansionReg;
				typename CalibrationParams::T::RegisterValueType TimerCCTL0Reg;
				typename CalibrationParams::T::RegisterValueType TimerCCR0Reg;
				typename CalibrationParams::T::RegisterValueType TimerCCTLxReg;
				typename CalibrationParams::T::RegisterValueType TimerCCRxReg;
				
				RegisterDataType BindRegister;
				RegisterDataType DividerRegister;
			};

			/// ��������� �������� ���������, ���������� �� ����� ����������, � ��������� RegistersState.
			static inline void GetRegistersState(RegistersState& state)
			{
				state.TimerCTLReg = CalibrationParams::T::GetControl();
				state.TimerExpansionReg = CalibrationParams::T::GetExpansion();
				state.TimerCCTL0Reg = CalibrationParams::T::CC0::GetControl();
				state.TimerCCR0Reg = CalibrationParams::T::CC0::GetValue();
				state.TimerCCTLxReg = CalibrationParams::CC::GetControl();
				state.TimerCCRxReg = CalibrationParams::CC::GetValue();

				state.BindRegister = _BindControl::BindRegister();
				state.DividerRegister = _BindControl::DividerRegister();
			}

			/// ������������ �������� ���������, ���������� �� ����� ����������, �� ��������� RegistersState.
			static inline void SetRegistersState(const RegistersState& state)
			{
				_BindControl::BindRegister() = state.BindRegister;
				_BindControl::DividerRegister() = state.DividerRegister;

				CalibrationParams::T::SetExpansion(state.TimerExpansionReg);
				CalibrationParams::T::CC0::SetValue(state.TimerCCR0Reg);
				CalibrationParams::CC::SetValue(state.TimerCCRxReg);
				CalibrationParams::CC::SetControl(state.TimerCCTLxReg);
				CalibrationParams::T::CC0::SetControl(state.TimerCCTL0Reg);
				CalibrationParams::T::Reset();
				CalibrationParams::T::SetControl(state.TimerCTLReg);
			}

		protected:

			/// �����-������ ��� ������� ��������� ������� DCO.
			/// \details ��������� ������ FrequencyCalibration.
			class Regulator
			{
			public:

				/// �������������.
				/// \details ������� ������ �������� ������� (RSEL). �������� ���������.
				static inline void Initialize(unsigned long requiredFreq)
				{
					_DcoControl::SetUnifiedTapSelect(0);
					_DcoControl::SetFrequencyRange(FrequencyRangeSelect::Calculate(requiredFreq));
					_DcoControl::EnableModulation();
				}

				/// ��������� �������.
				static inline bool IncrementFrequency()
				{
					if (_DcoControl::GetUnifiedTapSelect() == _DcoControl::MaxUnifiedTapSelect)
					{
						if (_DcoControl::GetFrequencyRange() == _DcoControl::MaxFrequencyRange())
							return false;
						_DcoControl::SetUnifiedTapSelect(0);
						_DcoControl::IncFrequencyRange();
					}
					else
						_DcoControl::IncUnifiedTapSelect();
					return true;
				}

				/// ��������� �������.
				static inline bool DecrementFrequency()
				{
					if (_DcoControl::GetUnifiedTapSelect() == 0)
					{
						if (_DcoControl::GetFrequencyRange() == 0)
							return false;
						_DcoControl::DecFrequencyRange();
						_DcoControl::SetUnifiedTapSelect(_DcoControl::MaxUnifiedTapSelect);
					}
					else
						_DcoControl::DecUnifiedTapSelect();
					return true;
				}
			};

		};

		/// ���������� �������� DCODIV.
		/// \details ��� �� DCO, �� ����������� ����� ��������.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType FllControlRegister1Address,
				PointerType FllControlRegister2Address>
		class InternalDivClockSourceImpl
		{
		protected:

			typedef FllControl<RegisterDataType, PointerType, FllControlRegister1Address, FllControlRegister2Address> _FllControl;

		public:

			/// ��������.
			enum Divider
			{
				/// 1
				Divider1 = _FllControl::FllDivider1,
				/// 2
				Divider2 = _FllControl::FllDivider2,
				/// 4
				Divider4 = _FllControl::FllDivider4,
				/// 8
				Divider8 = _FllControl::FllDivider8,
				/// 16
				Divider16 = _FllControl::FllDivider16,
				/// 32
				Divider32 = _FllControl::FllDivider32
			};

		public:

			/// ���������� �������� (Fdcodiv = Fdco / divider)
			static inline void SetDivider(Divider divider)
			{
				_FllControl::SetDivider(divider);
			}

		};

		/// ���������� �������� REFOCLK
		class InternalLowFreqClockSource1Impl
		{
		};

		/// ���������� �������� VLOCLK
		class InternalLowFreqClockSource2Impl
		{
		};

	}
}
}

#endif // CLOCK_SYSTEM_CLOCK_SOURCES_H
