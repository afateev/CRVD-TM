/////////////////////////////////////////
//
// Классы источников тактового сигнала
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

		/// \brief Внешний истоник XT1.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ExternalClockSource1Impl
		{
		protected:
			
			typedef GeneralSettingsControl<RegisterDataType, PointerType, ControlRegister1Address> SettingsControl;

		public:

			/// Максимальный допустимый потребляемый ток.
			/// \details От выбранного уровня зависит надёжность работы источника.
			enum CurrentAdjust
			{
				/// Самое низкое энергопотребление в режиме LF. Допустимая частота в режиме HF 4-8 Mhz.
				CurrentAdjustLevel1 = SettingsControl::CurrentAdjustLevel1,
				/// Повышенная надёжность в режиме LF. Допустимая частота в режиме HF 8-16 Mhz.
				CurrentAdjustLevel2 = SettingsControl::CurrentAdjustLevel2,
				/// Повышенная надёжность в режиме LF. Допустимая частота в режиме HF 16-24 Mhz.
				CurrentAdjustLevel3 = SettingsControl::CurrentAdjustLevel3,
				/// Максимальная надёжность и потребляемый тока в режиме LF. Допустимая частота в режиме HF 24-32 Mhz.
				CurrentAdjustLevel4 = SettingsControl::CurrentAdjustLevel4
			};

			/// Возможные значения ёмкостей.
			enum Capasitor
			{
				/// 2пФ
				Capasitor2Pf = SettingsControl::Capasitor2Pf,
				/// 6пФ
				Capasitor6Pf = SettingsControl::Capasitor6Pf,
				/// 9пФ
				Capasitor9Pf = SettingsControl::Capasitor9Pf,
				/// 12пФ
				Capasitor12Pf = SettingsControl::Capasitor12Pf
			};

		public:

			/// Включить источник.
			static inline void On()
			{
				SettingsControl::ExternalClock1On();
			}

			/// Выключить источник.
			static inline void Off()
			{
				SettingsControl::ExternalClock1Off();
			}

			/// Установить максимальный потребляемый ток.
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				SettingsControl::SetExternalClock1CurrentAdjust((typename SettingsControl::CurrentAdjust)currentAdjust);
			}

			/// Включить режим формирования высокочастотного тактового сигнала.
			static inline void HighFreqModeEnable()
			{
				SettingsControl::ExternalClock1HighFreqModeEnable();
			}

			/// Выключить режим формирования высокочастотного тактового сигнала.
			static inline void HighFreqModeDisable()
			{
				SettingsControl::ExternalClock1HighFreqModeDisable();
			}

			/// Разрешить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT1.
			static inline void PinModeEnable()
			{
				SettingsControl::ExternalClock1PinModeEnable();
			}

			/// Запретить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT1.
			static inline void PinModeDisable()
			{
				SettingsControl::ExternalClock1PinModeDisable();
			}

			/// Выбрать ёмкость.
			static inline void SetCapasitor(Capasitor capasitor)
			{
				SettingsControl::SetExternalClock1Capasitor((typename SettingsControl::Capasitor)capasitor);
			}

		};

		/// \brief Внешний истоник XT2.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ExternalClockSource2Impl
		{
		protected:
			
			typedef GeneralSettingsControl<RegisterDataType, PointerType, ControlRegister1Address> SettingsControl;

		public:

			/// Максимальный допустимый потребляемый ток.
			/// \details От выбранного уровня зависит надёжность работы источника.
			enum CurrentAdjust
			{
				/// Самое низкое энергопотребление в режиме LF. Допустимая частота в режиме HF 4-8 Mhz.
				CurrentAdjustLevel1 = SettingsControl::CurrentAdjustLevel1,
				/// Повышенная надёжность в режиме LF. Допустимая частота в режиме HF 8-16 Mhz.
				CurrentAdjustLevel2 = SettingsControl::CurrentAdjustLevel2,
				/// Повышенная надёжность в режиме LF. Допустимая частота в режиме HF 16-24 Mhz.
				CurrentAdjustLevel3 = SettingsControl::CurrentAdjustLevel3,
				/// Максимальная надёжность и потребляемый тока в режиме LF. Допустимая частота в режиме HF 24-32 Mhz.
				CurrentAdjustLevel4 = SettingsControl::CurrentAdjustLevel4
			};

		public:

			/// Включить источник.
			static inline void On()
			{
				SettingsControl::ExternalClock2On();
			}

			/// Выключить источник.
			static inline void Off()
			{
				SettingsControl::ExternalClock2Off();
			}

			/// Установить максимальный потребляемый ток.
			static inline void SetCurrentAdjust(CurrentAdjust currentAdjust)
			{
				SettingsControl::SetExternalClock2CurrentAdjust((typename SettingsControl::CurrentAdjust)currentAdjust);
			}

			/// Разрешить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT2.
			static inline void PinModeEnable()
			{
				SettingsControl::ExternalClock2PinModeEnable();
			}

			/// Запретить использовать в качестве тактового сигнала внешний сигнал, подаваемый на вход XT2.
			static inline void PinModeDisable()
			{
				SettingsControl::ExternalClock2PinModeDisable();
			}
		};

		/// Внутренний источник DCO.
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

			/// Предопределённые значния частоты источника.
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
				/// Максимальная частота (33MHz).
				FrequencyMax = Frequency33MHz,
				/// Максимальная частота ядра (25MHz).
				FrequencyMaxCore = Frequency25MHz,
				/// Частота по умолчанию (16MHz).                  
                FrequencyDefault = Frequency16MHz
			};

		public:

			/// Включить FLL.
			static inline void EnableFll()
			{
				OperatingMode::EnableFll();
			}

			/// Выключить FLL.
			static inline void DisableFll()
			{
				OperatingMode::DisableFll();
			}

			/// Проверить включен ли FLL.
			static inline bool IsFllOn()
			{
				return OperatingMode::IsFllEnabled();
			}

			/// Установить частоту источника, используя возможности аппаратного FLL.
			/// \param referenceSource Истоник эталонного тактового сигнала для FLL.
			/// \param sourceFreq Частота эталонного сигнала.
			/// \param requiredFreq Частота, на которую надо настроить источник.
			/// \param fllDivider Делитель эталонной частоты FLL.
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

			/// Установить частоту источника, используя процедуру калибровки.
			/// \details Эта функция не учитывает текущие установки частоты.
			/// При каждом вызове работает одинаково долго (CorrectFrequency при повторных вызовах работает существенно быстрее).
			/// \param referenceClockSource Истоник эталонного тактового сигнала.
			/// \param referenceSourceFreq Частота эталонного сигнала.
			/// \param requiredFreq Частота, на которую надо настроить источник.
			static unsigned long SetFrequency(typename _BindControl::ClockSource referenceClockSource, // источник эталонной частоты
												unsigned long referenceSourceFreq, // значение эталонной частоты
												unsigned long requiredFreq) // требуемая частота DCO
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

				Regulator::Initialize(requiredFreq); // установить начальные значения
				unsigned long resultFrequency = _FrequencyCalibration::CorrectFrequency(_BindControl::ClockSourceInternal
																					, referenceClockSource
																					, referenceSourceFreq
																					, requiredFreq);

				SetRegistersState(state);

				return resultFrequency;
			}

			/// Установить частоту источника, используя процедуру калибровки.
			/// \details Эта функция "отталкивается" от текущих установок.
			/// Работает быстро при повторных вызовах. При первом вызове в общем случае работает дольше, чем SetFrequency.
			/// \param referenceClockSource Истоник эталонного тактового сигнала.
			/// \param referenceSourceFreq Частота эталонного сигнала.
			/// \param requiredFreq Частота, на которую надо настроить источник.
			static unsigned long CorrectFrequency(typename _BindControl::ClockSource referenceClockSource, // источник эталонной частоты
												unsigned long referenceSourceFreq, // значение эталонной частоты
												unsigned long requiredFreq) // требуемая частота DCO
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

				// не устанавливаем начальные значения, начинаем с текущих установок
				unsigned long resultFrequency = _FrequencyCalibration::CorrectFrequency(_BindControl::ClockSourceInternal
																					, referenceClockSource
																					, referenceSourceFreq
																					, requiredFreq);

				SetRegistersState(state);

				return resultFrequency;
			}
			
			/// Определить частоту источника.
			/// \param referenceClockSource Источник эталонной частоты.
			/// \param referenceSourceFreq Частота эталонного источника.
			static unsigned long CalcFrequency(typename _BindControl::ClockSource referenceClockSource, // источник эталонной частоты
												unsigned long referenceSourceFreq) // значение эталонной частоты
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

			/// Структура для хранения состояния регистров таймера и системы тактирования во время калибровки.
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

			/// Сохранить значения регистров, изменяемые во время калибровки, в структуре RegistersState.
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

			/// Восстановить значения регистров, изменяемые во время калибровки, из структуре RegistersState.
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

			/// Класс-обёртка для методов настройки частоты DCO.
			/// \details Передаётся классу FrequencyCalibration.
			class Regulator
			{
			public:

				/// Инициализация.
				/// \details Выбрать нужный диапазон частоты (RSEL). Включить модуляцию.
				static inline void Initialize(unsigned long requiredFreq)
				{
					_DcoControl::SetUnifiedTapSelect(0);
					_DcoControl::SetFrequencyRange(FrequencyRangeSelect::Calculate(requiredFreq));
					_DcoControl::EnableModulation();
				}

				/// Увеличить частоту.
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

				/// Уменьшить частоту.
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

		/// Внутренний источник DCODIV.
		/// \details Тот же DCO, но пропущенный через делитель.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType FllControlRegister1Address,
				PointerType FllControlRegister2Address>
		class InternalDivClockSourceImpl
		{
		protected:

			typedef FllControl<RegisterDataType, PointerType, FllControlRegister1Address, FllControlRegister2Address> _FllControl;

		public:

			/// Делители.
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

			/// установить делитель (Fdcodiv = Fdco / divider)
			static inline void SetDivider(Divider divider)
			{
				_FllControl::SetDivider(divider);
			}

		};

		/// внутренний источник REFOCLK
		class InternalLowFreqClockSource1Impl
		{
		};

		/// внутренний источник VLOCLK
		class InternalLowFreqClockSource2Impl
		{
		};

	}
}
}

#endif // CLOCK_SYSTEM_CLOCK_SOURCES_H
