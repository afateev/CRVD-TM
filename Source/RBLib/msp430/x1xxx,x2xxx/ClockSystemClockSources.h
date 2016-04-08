
#ifndef CLOCK_SYSTEM_CLODK_SOURCES_MSP430X1XXX_MSP430X2XXX_H
#define CLOCK_SYSTEM_CLODK_SOURCES_MSP430X1XXX_MSP430X2XXX_H

#include "ClockSystemControl.h"
#include "../FrequencyCalibration.h"

namespace Rblib
{
namespace Msp430x12xxx
{
	namespace ClockSystemParts
	{
		/// \brief ������� �������� XT1.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� BCSCTL1.
		/// \tparam ControlRegister3Address ����� �������� BCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ExternalClockSource1Impl
		{

		protected:

			typedef Control1<RegisterDataType, PointerType, ControlRegister1Address> _Control1;

		public:

			static inline void On()
			{
			}

			/// �������� ����� �� (HF).
			static inline void HighFreqModeEnable()
			{
				_Control1::ExternalClock1HighFreqModeEnable();
			}

			/// ��������� ����� �� (HF).
			static inline void HighFreqModeDisable()
			{
				_Control1::ExternalClock1HighFreqModeDisable();
			}

		};

		/// \brief ������� �������� XT2.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� BCSCTL1.
		/// \tparam ControlRegister3Address ����� �������� BCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ExternalClockSource2Impl
		{

		protected:

			typedef Control1<RegisterDataType, PointerType, ControlRegister1Address> _Control1;

		public:

			/// ��������.
			static inline void On()
			{
				_Control1::ExternalClock2Enable();
			}

			/// ���������.
			/// \details ���� XT2 ������������ ��� ������������ MCLK ��� SMCLK, �� �� ����������.
			static inline void Off()
			{
				_Control1::ExternalClock2Disable();
			}
		};

		/// \brief ���������� �������� DCO.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam DcoControlRegisterAddress ����� �������� DCOCTL.
		/// \tparam ControlRegister1Address ����� �������� BCSCTL1.
		/// \tparam ControlRegister2Address ����� �������� BCSCTL2.
		/// \tparam ControlRegister3Address ����� �������� BCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address,
				PointerType ControlRegister2Address,
        class ActualDcoControl,
				class BindControl,
				class FrequencyRangeSelect,
				class CalibrationParams>
		class InternalClockSourceImpl
			: public ActualDcoControl
		{

		protected:
			
			class Regulator;

			typedef ActualDcoControl _DcoControl;
			typedef Control1<RegisterDataType, PointerType, ControlRegister1Address> _Control1;
			typedef Control2<RegisterDataType, PointerType, ControlRegister2Address> _Control2;

			typedef Msp430::ClockSystemParts::FrequencyCalibration<BindControl, CalibrationParams, Regulator> _FrequencyCalibration;

		public:

			/// ��������������� ������� �������.
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
				/// ������������ ������� (16MHz).
				FrequencyMax = Frequency16MHz,
                FrequencyDefault = Frequency16MHz
			};

		public:

			/// ������������ ������� ��������.
			static inline void SetResistorExternal()
			{
				_Control2::SetInternalSourceResistorExternal();
			}

			/// ������������ ���������� ��������.
			static inline void SetResistorInternal()
			{
				_Control2::SetInternalSourceResistorInternal();
			}

			/// ��������� ����� �������� ������������.
			/// return @c true, ���� ������������ ������� �������� (DCOR=1), ��� @c false, ���� ������������ ���������� �������� (DCOR=0).
			static inline bool ResistorExternal()
			{
				return _Control2::InternalSourceResistorExternal();
			}

			/// ���������� ������� ���������, ��������� ��������� ����������.
			/// \details ��� ������� �� ��������� ������� ��������� �������.
			/// ��� ������ ������ �������� ��������� ����� (CorrectFrequency ��� ��������� ������� �������� ����������� �������).
			/// \param referenceClockSource ������� ���������� ��������� �������.
			/// \param referenceSourceFreq ������� ���������� �������.
			/// \param requiredFreq �������, �� ������� ���� ��������� ��������.
			static unsigned long SetFrequency(typename BindControl::ClockSource referenceClockSource, // �������� ��������� �������
												unsigned long referenceSourceFreq, // �������� ��������� �������
												unsigned long requiredFreq) // ��������� ������� DCO
			{
				if (0 == referenceSourceFreq || 0 == requiredFreq)
					return 0;

				if (requiredFreq > FrequencyMax)
					requiredFreq = FrequencyMax;

				RegistersState state;
				GetRegistersState(state);

				Regulator::Initialize(requiredFreq); // ���������� ��������� ��������
				unsigned long resultFrequency = _FrequencyCalibration::CorrectFrequency(BindControl::ClockSourceInternal
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
			static unsigned long CorrectFrequency(typename BindControl::ClockSource referenceClockSource, // �������� ��������� �������
												unsigned long referenceSourceFreq, // �������� ��������� �������
												unsigned long requiredFreq) // ��������� ������� DCO
			{
				if (0 == referenceSourceFreq || 0 == requiredFreq)
					return 0;

				if (requiredFreq > FrequencyMax)
					requiredFreq = FrequencyMax;

				RegistersState state;
				GetRegistersState(state);

				// �� ������������� ��������� ��������, �������� � ������� ���������
				unsigned long resultFrequency = _FrequencyCalibration::CorrectFrequency(BindControl::ClockSourceInternal
																					, referenceClockSource
																					, referenceSourceFreq
																					, requiredFreq);

				SetRegistersState(state);

				return resultFrequency;
			}

			/// ���������� ������� ���������.
			/// \param referenceClockSource �������� ��������� �������.
			/// \param referenceSourceFreq ������� ���������� ���������.
			static unsigned long CalcFrequency(typename BindControl::ClockSource referenceClockSource, // �������� ��������� �������
												unsigned long referenceSourceFreq) // �������� ��������� �������
			{
				if (0 == referenceSourceFreq)
					return 0;

				RegistersState state;
				GetRegistersState(state);

				unsigned long resultFrequency = _FrequencyCalibration::CalcFrequency(BindControl::ClockSourceInternal
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
				typename CalibrationParams::T::RegisterValueType TimerCCTL0Reg;
				typename CalibrationParams::T::RegisterValueType TimerCCR0Reg;
				typename CalibrationParams::T::RegisterValueType TimerCCTLxReg;
				typename CalibrationParams::T::RegisterValueType TimerCCRxReg;
				
				RegisterDataType ControlRegister1Value;
				RegisterDataType ControlRegister2Value;
			};

			/// ��������� �������� ���������, ���������� �� ����� ����������, � ��������� RegistersState.
			static inline void GetRegistersState(RegistersState& state)
			{
				state.TimerCTLReg = CalibrationParams::T::GetControl();
				state.TimerCCTL0Reg = CalibrationParams::T::CC0::GetControl();
				state.TimerCCR0Reg = CalibrationParams::T::CC0::GetValue();
				state.TimerCCTLxReg = CalibrationParams::CC::GetControl();
				state.TimerCCRxReg = CalibrationParams::CC::GetValue();

				state.ControlRegister1Value = _Control1::GetControlRegister1();
				state.ControlRegister2Value = _Control2::GetControlRegister2();
			}

			/// ������������ �������� ���������, ���������� �� ����� ����������, �� ��������� RegistersState.
			static inline void SetRegistersState(const RegistersState& state)
			{
				_Control1::GetControlRegister1() = ((state.ControlRegister1Value & 0xF0) | _Control1::GetInternalClockFrequencyRange());
				_Control2::GetControlRegister2() = state.ControlRegister2Value;

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
				/// \details ������� ������ �������� ������� (RSEL).
				static inline void Initialize(unsigned long requiredFreq)
				{
					_DcoControl::SetUnifiedTapSelect(0);
					_DcoControl::SetFrequencyRange(FrequencyRangeSelect::Calculate(requiredFreq));
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

	}
}
}

#endif // CLOCK_SYSTEM_CLODK_SOURCES_MSP430X1XXX_MSP430X2XXX_H
