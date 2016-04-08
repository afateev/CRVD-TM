
#ifndef CLOCK_SYSTEM_CLODK_SOURCES_MSP430X2XX_H
#define CLOCK_SYSTEM_CLODK_SOURCES_MSP430X2XX_H

#include "ClockSystemControl.h"
#include "../FrequencyCalibration.h"
#include "../x1xxx,x2xxx/ClockSystemClockSources.h"

namespace Rblib
{
namespace Msp430x2xx
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
				PointerType ControlRegister1Address,
				PointerType ControlRegister3Address>
    class ExternalClockSource1Impl : public Msp430x12xxx::ClockSystemParts::ExternalClockSource1Impl<RegisterDataType, PointerType, ControlRegister1Address>
		{

		protected:

            typedef Msp430x12xxx::ClockSystemParts::ExternalClockSource1Impl<RegisterDataType, PointerType, ControlRegister1Address> _Base;
			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		public:

			/// ���� ������������ ��� (��� ����, ��� ����������)
			enum CurrentAdjust
			{
				/// ��� ������ 0.4 - 1 MHz.
				CurrentAdjustLevel1 = _Control3::ExternalSourceMode1,
				/// ��� ����� 1 - 3 MHz.
				CurrentAdjustLevel2 = _Control3::ExternalSourceMode2,
				/// ��� ������ 3 - 16 MHz.
				CurrentAdjustLevel3 = _Control3::ExternalSourceMode3
			};


			/// �������� ��������.
			enum Capasitor
			{
				/// 1��.
				Capasitor1Pf = _Control3::Capasitor1Pf,
				/// 6��.
				Capasitor6Pf = _Control3::Capasitor6Pf,
				/// 10��.
				Capasitor10Pf = _Control3::Capasitor10Pf,
				/// 12��.
				Capasitor12Pf = _Control3::Capasitor12Pf
			};

		public:

			/// ���������� ����. ������������ ��� (�������� ������ � ������ ��).
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				if (_Base::_Control1::ExternalClock1HighFreqModeEnabled())
					_Control3::SetExternalClock1Mode((typename _Control3::ExternlaSourceMode) currentAdjust);
			}

			/// ������������� �� ������� �������� ������.
			static inline void PinModeEnable()
			{
				_Control3::SetExternalClock1Mode(_Control3::ExternlaSourceMode4);
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT1.
			static inline void PinModeDisable()
			{
				_Control3::SetExternalClock1Mode(_Control3::ExternalSourceMode1);
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT1.
			static inline void SetCapasitor(Capasitor capasitor)
			{
				_Control3::SetExternalClock1Capasitor((typename _Control3::Capasitor) capasitor);
			}

		};

		/// \brief ������� �������� XT2.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� BCSCTL1.
		/// \tparam ControlRegister3Address ����� �������� BCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address,
				PointerType ControlRegister3Address>
		class ExternalClockSource2Impl : public Msp430x12xxx::ClockSystemParts::ExternalClockSource2Impl<RegisterDataType, PointerType, ControlRegister1Address>
		{

		protected:

			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		public:

			/// ���� ������������ ��� (��� ����, ��� ����������)
			enum CurrentAdjust
			{
				/// ��� ������ 0.4 - 1 MHz.
				CurrentAdjustLevel1 = _Control3::ExternalSourceMode1,
				/// ��� ����� 1 - 3 MHz.
				CurrentAdjustLevel2 = _Control3::ExternalSourceMode2,
				/// ��� ������ 3 - 16 MHz.
				CurrentAdjustLevel3 = _Control3::ExternalSourceMode3
			};

		public:

			/// ���������� ����. ������������ ���.
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				_Control3::SetExternalClock2CurrentAdjust(currentAdjust);
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT2.
			static inline void PinModeEnable()
			{
				_Control3::SetExternalClock2Mode(_Control3::ExternlaSourceMode4);
			}

			/// ��������� ������������ � �������� ��������� ������� ������� ������, ���������� �� ���� XT2.
			static inline void PinModeDisable()
			{
				_Control3::SetExternalClock2Mode(_Control3::ExternlaSourceMode1);
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
				PointerType DcoControlRegisterAddress,
				PointerType ControlRegister1Address,
				PointerType ControlRegister2Address,
				PointerType ControlRegister3Address,
				class FrequencyRangeSelect,
				class CalibrationParams>
    class InternalClockSourceImpl : public Msp430x12xxx::ClockSystemParts::InternalClockSourceImpl<RegisterDataType,
                                                                                                    PointerType,
                                                                                                    ControlRegister1Address,
                                                                                                    ControlRegister2Address,
                                                                                                    DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address>,
                                                                                                    ClockSourceBindControl<RegisterDataType, PointerType, ControlRegister1Address, ControlRegister2Address, ControlRegister3Address>,
                                                                                                    FrequencyRangeSelect,
                                                                                                    CalibrationParams>
		{
		public:

		};

		/// ���������� �������� VLOCLK
		/// \details VLOCLK - ����� ������ XT1.
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam ControlRegister1Address ����� �������� BCSCTL1.
		/// \tparam ControlRegister3Address ����� �������� BCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address,
				PointerType ControlRegister3Address>
		class InternalLowFreqClockSource1Impl
		{
		protected:

      typedef Msp430x12xxx::ClockSystemParts::Control1<RegisterDataType, PointerType, ControlRegister1Address> _Control1;
			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		protected:

			static bool _highFreqMode;
			static typename _Control3::ExternlaSourceMode _lastExternalSource1Mode;

		public:

			/// �������� ��������.
			/// \details ��������� XT1 � ����� ������������ �� VLO. ��������� ������� ���������.
			static void On()
			{
				if (_Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3 
					&& _Control1::ExternalClock1HighFreqModeDisabled())
				{
					return;
				}

				_highFreqMode = _Control1::ExternalClock1HighFreqModeEnabled();
				_lastExternalSource1Mode = _Control3::GetExternalClock1Mode();

				_Control1::ExternalClock1HighFreqModeDisable();
				_Control3::SetExternalClock1Mode(_Control3::ExternalSourceMode3);
			}

			/// ��������� ��������.
			/// \details ��������� XT1 � ������� ���������, ���������� ��� ��������� VLO.
			static inline void Off()
			{
				_Control1::SetExternalClock1HighFreqMode(_highFreqMode);
				_Control3::SetExternalClock1Mode(_lastExternalSource1Mode);
			}

		};

		//
		// ������������� ����������� ����������
		//
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister3Address>
		bool InternalLowFreqClockSource1Impl<RegisterDataType, 
											PointerType, 
											ControlRegister1Address, 
											ControlRegister3Address>
			::_highFreqMode = false;

		
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister3Address>
				  
		typename InternalLowFreqClockSource1Impl<RegisterDataType, 
												PointerType, 
												ControlRegister1Address, 
												ControlRegister3Address>
			::_Control3::ExternlaSourceMode
																				  
		InternalLowFreqClockSource1Impl<RegisterDataType, 
										PointerType, 
										ControlRegister1Address, 
										ControlRegister3Address>
			::_lastExternalSource1Mode = _Control3::ExternalSourceMode1;

	}
}
}

#endif // CLOCK_SYSTEM_CLODK_SOURCES_MSP430X2XX_H
