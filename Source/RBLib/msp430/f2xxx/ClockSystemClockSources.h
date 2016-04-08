
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
		/// \brief Внешний истончик XT1.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра BCSCTL1.
		/// \tparam ControlRegister3Address Адрес регистра BCSCTL3.
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

			/// Макс потребляемый ток (чем выше, тем стабильнее)
			enum CurrentAdjust
			{
				/// Для частот 0.4 - 1 MHz.
				CurrentAdjustLevel1 = _Control3::ExternalSourceMode1,
				/// Для чатот 1 - 3 MHz.
				CurrentAdjustLevel2 = _Control3::ExternalSourceMode2,
				/// Для частот 3 - 16 MHz.
				CurrentAdjustLevel3 = _Control3::ExternalSourceMode3
			};


			/// Значения ёмкостей.
			enum Capasitor
			{
				/// 1пФ.
				Capasitor1Pf = _Control3::Capasitor1Pf,
				/// 6пФ.
				Capasitor6Pf = _Control3::Capasitor6Pf,
				/// 10пФ.
				Capasitor10Pf = _Control3::Capasitor10Pf,
				/// 12пФ.
				Capasitor12Pf = _Control3::Capasitor12Pf
			};

		public:

			/// Установить макс. потребляемый ток (работает только в режиме ВЧ).
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				if (_Base::_Control1::ExternalClock1HighFreqModeEnabled())
					_Control3::SetExternalClock1Mode((typename _Control3::ExternlaSourceMode) currentAdjust);
			}

			/// Переключиться на внешний тактовый сигнал.
			static inline void PinModeEnable()
			{
				_Control3::SetExternalClock1Mode(_Control3::ExternlaSourceMode4);
			}

			/// Разрешить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT1.
			static inline void PinModeDisable()
			{
				_Control3::SetExternalClock1Mode(_Control3::ExternalSourceMode1);
			}

			/// Запретить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT1.
			static inline void SetCapasitor(Capasitor capasitor)
			{
				_Control3::SetExternalClock1Capasitor((typename _Control3::Capasitor) capasitor);
			}

		};

		/// \brief Внешний источник XT2.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра BCSCTL1.
		/// \tparam ControlRegister3Address Адрес регистра BCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address,
				PointerType ControlRegister3Address>
		class ExternalClockSource2Impl : public Msp430x12xxx::ClockSystemParts::ExternalClockSource2Impl<RegisterDataType, PointerType, ControlRegister1Address>
		{

		protected:

			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		public:

			/// Макс потребляемый ток (чем выше, тем стабильнее)
			enum CurrentAdjust
			{
				/// Для частот 0.4 - 1 MHz.
				CurrentAdjustLevel1 = _Control3::ExternalSourceMode1,
				/// Для чатот 1 - 3 MHz.
				CurrentAdjustLevel2 = _Control3::ExternalSourceMode2,
				/// Для частот 3 - 16 MHz.
				CurrentAdjustLevel3 = _Control3::ExternalSourceMode3
			};

		public:

			/// Установить макс. потребляемый ток.
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				_Control3::SetExternalClock2CurrentAdjust(currentAdjust);
			}

			/// Разрешить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT2.
			static inline void PinModeEnable()
			{
				_Control3::SetExternalClock2Mode(_Control3::ExternlaSourceMode4);
			}

			/// Запретить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT2.
			static inline void PinModeDisable()
			{
				_Control3::SetExternalClock2Mode(_Control3::ExternlaSourceMode1);
			}
		};

		/// \brief Внутренний источник DCO.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam DcoControlRegisterAddress Адрес регистра DCOCTL.
		/// \tparam ControlRegister1Address Адрес регистра BCSCTL1.
		/// \tparam ControlRegister2Address Адрес регистра BCSCTL2.
		/// \tparam ControlRegister3Address Адрес регистра BCSCTL3.
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

		/// внутренний источник VLOCLK
		/// \details VLOCLK - режим работы XT1.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра BCSCTL1.
		/// \tparam ControlRegister3Address Адрес регистра BCSCTL3.
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

			/// Включить источник.
			/// \details Переводит XT1 в режим тактирования от VLO. Сохраняет текущие настройки.
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

			/// Выключить источник.
			/// \details Переводит XT1 в прежнее состояние, сохранённое при включении VLO.
			static inline void Off()
			{
				_Control1::SetExternalClock1HighFreqMode(_highFreqMode);
				_Control3::SetExternalClock1Mode(_lastExternalSource1Mode);
			}

		};

		//
		// Инициализация статических переменных
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
