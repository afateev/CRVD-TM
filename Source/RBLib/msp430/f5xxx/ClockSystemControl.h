///////////////////////////////////////////////////
//
// Классы доступа к регистрам системы тактирования
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
		/// Класс управления регистрами DCO (UCSCTL0, UCSCTL1).
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра UCSCTL0.
		/// \tparam ControlRegister2Address Адрес регистра UCSCTL1.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class DcoControl
		{
		public:

			/// Константы.
			enum Constants
			{
				/// Максимальнаое значение регистра управляющего частотой (UCSCTL0, поля DCO + MOD).
				MaxUnifiedTapSelect = 0x3FF,
			};

		protected:

			/// Указатель на регистр UCSCTL0.
			static volatile DcoControlRegister1* const _controlRegister1Addr;
			/// Указатель на регистр UCSCTL1.
			static volatile DcoControlRegister2* const _controlRegister2Addr;

		public:
			
			/// Возвращает количество диапазонов частоты.
			/// \details максимальнгое значение поля DCORSEL в регистре UCSCTL1.
			static inline unsigned char MaxFrequencyRange()
			{
				return 7;
			}

			/// Указатель на регистр UCSCTL0.
			static inline RegisterDataType& DcoRegister1()
			{
				return (RegisterDataType&) *_controlRegister1Addr;
			}

			/// Указатель на регистр UCSCTL1.
			static inline RegisterDataType& DcoRegister2()
			{
				return (RegisterDataType&) *_controlRegister2Addr;
			}

			/// Включить модуляцию.
			static inline void EnableModulation()
			{
				_controlRegister2Addr->ModulationEnable = 0;
			}

			/// Выключить модуляцию.
			static inline void DisableModulation()
			{
				_controlRegister2Addr->ModulationEnable = 1;
			}

			/// Возвращает значение поля MOD регистра UCSCTL0.
			static inline RegisterDataType GetModulationCounter()
			{
				return _controlRegister1Addr->ModulationCounter;
			}

			/// Устанавливает значение поля MOD регистра UCSCTL0.
			static inline void SetModulationCounter(RegisterDataType modulationCounter)
			{
				_controlRegister1Addr->ModulationCounter = modulationCounter;
			}

			/// Возвращает значение поля DCO регистра UCSCTL0.
			static inline RegisterDataType GetTapSelect()
			{
				return _controlRegister1Addr->TapSelect;
			}

			/// Устанавливает значение поля DCO регистра UCSCTL0.
			static inline void SetTapSelect(RegisterDataType tapSelect)
			{
				_controlRegister1Addr->TapSelect = tapSelect;
			}

			/// Возвращает поля DCO и MOD регистра UCSCTL0, как одно значение.
			static inline RegisterDataType GetUnifiedTapSelect()
			{
				return _controlRegister1Addr->UnifiedTapField;
			}

			/// Устанавливает поля DCO и MOD регистра UCSCTL0, как одно значение.
			static inline void SetUnifiedTapSelect(RegisterDataType tapSelect)
			{
				_controlRegister1Addr->UnifiedTapField = tapSelect;
			}

			/// Увеличивает на 1 объединённое поле DCO + MOD.
			static inline void IncUnifiedTapSelect()
			{
				_controlRegister1Addr->UnifiedTapField++;
			}

			/// Уменьшает на 1 объединённое поле DCO + MOD.
			static inline void DecUnifiedTapSelect()
			{
				_controlRegister1Addr->UnifiedTapField--;
			}

			/// Возвращает значние поля DCORSEL регистра UCSCTL1.
			static inline RegisterDataType GetFrequencyRange()
			{
				return _controlRegister2Addr->FrequencyRange;
			}

			/// Устанавливает значние поля DCORSEL регистра UCSCTL1.
			static inline void SetFrequencyRange(RegisterDataType frequencyRange)
			{
				_controlRegister2Addr->FrequencyRange = frequencyRange;
			}

			/// Увеличивает на 1 значние поля DCORSEL регистра UCSCTL1.
			static inline void IncFrequencyRange()
			{
				_controlRegister2Addr->FrequencyRange++;
			}

			/// Уменьшает на 1 значние поля DCORSEL регистра UCSCTL1.
			static inline void DecFrequencyRange()
			{
				_controlRegister2Addr->FrequencyRange--;
			}
		};

		/// Класс управления регистрами FLL (UCSCTL2, UCSCTL3)
		/// \details FLL предназначен для автоматической подстройки частоты DCO в реальном времени.
		/// \details Значение частоты, которую будет поддерживать FLL, определяется частотой эталонного сигнала, 
		/// \details делителем частоты эталонного сигнала, и дувумя параметрами FLL: умножителем и делителем.
		/// \details Fdco = Dfll * (Mfll + 1) * (Fref / Dref)\n
		/// \details Fdcodv = (Mfll + 1) * (Fref / Dref)\n
		/// \details Dfll - делитель Fll.
		/// \details Mfll - множитель Fll.
		/// \details Fref - частота эталонного сигнала.
		/// \details Dref - делитель частоты эталонного сигнала.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра UCSCTL2.
		/// \tparam ControlRegister2Address Адрес регистра UCSCTL3.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class FllControl
		{
		public:

			/// Возможные эталонные источники.
			enum FllReferenceSelect
			{
				/// Внешний источник XT1.
				FllReferenceSelectExternlaClock1 = 0,
				/// Веншний источник XT2.
				FllReferenceSelectExternlaClock2 = 5,
				/// Внетренний REFOCLK.
				FllReferenceSelectInternalLowFreqClock = 2,
				/// Источник не выбран.
				FllReferenceSelectNone = 7
			};

			/// Делители эталонной частоты.
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

			/// Делители Fll (Dfll).
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

			/// Константы.
			enum Consants
			{
				/// Максимальное значение множителя Fll (Mfll).
				FllMaxMultiplierValue = 1023
			};


		protected:

			/// Указатель на регистр UCSCTL2.
			static volatile FllControlRegister1 * const _controlRegister1Addr;
			/// Указатель на регистр UCSCTL3.
			static volatile FllControlRegister2 * const _controlRegister2Addr;

		public:

			/// Прямой доступ к регистру UCSCTL2.
			static inline RegisterDataType& FllRegister1()
			{
				return *_controlRegister1Addr;
			}

			/// Прямой доступ к регистру UCSCTL3.
			static inline RegisterDataType& FllRegister2()
			{
				return *_controlRegister2Addr;
			}
			
			/// Возвращает значение делителя Fll.
			static inline FllDivider GetFllDivider()
			{
				return (FllDivider) _controlRegister1Addr->Divider;
			}

			/// Устанавливает значение делителя Fll.
			/// \param divider Значение делителя.
			static inline void SetFllDivider(FllDivider divider)
			{
				_controlRegister1Addr->Divider = divider;
			}

			/// Возвращает значение множителя Fll.
			static inline RegisterDataType GetFllMultiplier()
			{
				return _controlRegister1Addr->Multiplier;
			}

			/// Устанавливает значение множителя Fll.
			/// \param multiplier Значение множителя.
			static inline void SetFllMultiplier(RegisterDataType multiplier)
			{
				_controlRegister1Addr->Multiplier = multiplier;
			}

			/// Задать источник эталонной частоты.
			/// \param reference Источник эталонной частоты.
			static inline void SetFllReference(FllReferenceSelect reference)
			{
				_controlRegister2Addr->ReferenceSelect = reference;
			}

			/// Задать делитель эталонной частоты.
			/// \param divider Делитель источника эталонной частоты.
			static inline void SetFllReferenceDivider(FllReferenceDivider divider)
			{
				_controlRegister2Addr->ReferenceDivider = divider;
			}

		public:

			/// Преобразовать делитель Fll из енама в число.
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

			/// Преобразовать делитель Fll из в числа в енам.
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

			/// Преобразовать делитель эталонной частоты из енама в число.
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

			/// Преобразовать делитель эталонной частоты из в числа в енам.
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

		/// Класс предоставляющий список констант для идентификации источников тактирования
		/// \details Нуждающиеся в константантах классы должны ВИРТУАЛЬНО наследоваться от ClockSourceNamesProvider
		class ClockSourceNamesProvider
		{
		public:

			/// Список констант идентифицирующих источники.
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

		/// Класс доступа к регистру привязки истоников к сигналам и регистру настройки делителей (UCSCTL4, UCSCTL5)
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра UCSCTL4.
		/// \tparam ControlRegister2Address Адрес регистра UCSCTL5.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class ClockSourceBindControl : virtual public ClockSourceNamesProvider
		{

		public:

			/// Делители.
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

			/// Указатель на регистр UCSCTL4.
			static volatile ClockSourceSelectRegister * const _bindControlRegisterAddr;
			/// Указатель на регистр UCSCTL5.
			static volatile ClockDividerRegister * const _dividerControlRegisterAddr;

		public:

			/// Преобразует значение делителя из числа в енам, округляя в меньшуую сторону.
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

			/// Преобразует значение делителя из числа в енам, окрудгяя в большую сторону.
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

			/// Преобразует значение делителя из енама в число.
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

			/// Доступ к регистру UCSCTL4
			static inline RegisterDataType& BindRegister()
			{
				return (RegisterDataType&) *_bindControlRegisterAddr;
			}

			/// Доступ к регистру UCSCTL5
			static inline RegisterDataType& DividerRegister()
			{
				return (RegisterDataType&) *_dividerControlRegisterAddr;
			}

			// источники

			/// Установить источник для сигнала MCLK.
			static inline void SetCoreSignalSource(ClockSource source)
			{
				_bindControlRegisterAddr->CoreClockSource = source;
			}

			/// Установить источник для сигнала SMCLK.
			static inline void SetPeripherySignalSource(ClockSource source)
			{
				_bindControlRegisterAddr->PeripheryClockSource = source;
			}
			
			/// Установить источник для сигнала ACLK.
			static inline void SetPeripheryAuxSignalSource(ClockSource source)
			{
				_bindControlRegisterAddr->PeripheryAuxClockSource = source;
			}

			// делители

			/// Установить делитель для сигнала MCLK.
			static inline void SetCoreSignalDivider(Divider divider)
			{
				_dividerControlRegisterAddr->CoreClockDivider = divider;
			}

			/// Установить делитель для сигнала SMCLK.
			static inline void SetPeripherySignalDivider(Divider divider)
			{
				_dividerControlRegisterAddr->PeripheryClockDivider = divider;
			}
			
			/// Установить делитель для сигнала ACLK.
			static inline void SetPeripheryAuxSignalDivider(Divider divider)
			{
				_dividerControlRegisterAddr->PeripheryAuxClockDivider = divider;
			}

			/// Установить делитель для сигнала MCLK.
			static inline void SetCoreSignalDivider(unsigned long divider)
			{
				_dividerControlRegisterAddr->CoreClockDivider = GetDivider(divider);
			}

			/// Установить делитель для сигнала SMCLK.
			static inline void SetPeripherySignalDivider(unsigned long divider)
			{
				_dividerControlRegisterAddr->PeripheryClockDivider = GetDivider(divider);
			}
			
			/// Установить делитель для сигнала ACLK.
			static inline void SetPeripheryAuxSignalDivider(unsigned long divider)
			{
				_dividerControlRegisterAddr->PeripheryAuxClockDivider = GetDivider(divider);
			}
		};

		/// Класс управления регистром общих настроек источников тактирования (UCSCTL6).
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра UCSCTL6.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class GeneralSettingsControl
		{
		public:

			/// Максимальный допустимый потребляемый ток.
			/// \details От выбранного уровня зависит надёжность работы источника.
			enum CurrentAdjust
			{
				/// Самое низкое энергопотребление в режиме LF. Допустимая частота в режиме HF 4-8 Mhz.
				CurrentAdjustLevel1 = 0,
				/// Повышенная надёжность в режиме LF. Допустимая частота в режиме HF 8-16 Mhz.
				CurrentAdjustLevel2 = 1,
				/// Повышенная надёжность в режиме LF. Допустимая частота в режиме HF 16-24 Mhz.
				CurrentAdjustLevel3 = 2,
				/// Максимальная надёжность и потребляемый тока в режиме LF. Допустимая частота в режиме HF 24-32 Mhz.
				CurrentAdjustLevel4 = 3
			};

			/// Возможные значения ёмкостей.
			enum Capasitor
			{
				/// 2пФ
				Capasitor2Pf = 0,
				/// 6пФ
				Capasitor6Pf = 1,
				/// 9пФ
				Capasitor9Pf = 2,
				/// 12пФ
				Capasitor12Pf = 3
			};

		protected:

			/// Указатель на регистр UCSCTL6.
			static volatile GeneralSettingsRegister * const _generalSettingsRegisterAddr;

		public:
			
			/// Включить источник XT1.
			static inline void ExternalClock1On()
			{
				_generalSettingsRegisterAddr->ExternalSource1Off = 0;
			}

			/// Выключить источник XT1.
			static inline void ExternalClock1Off()
			{
				_generalSettingsRegisterAddr->ExternalSource1Off = 1;
			}

			/// Установить уровень энергопотребления источника XT1.
			static inline void SetExternalClock1CurrentAdjust(CurrentAdjust currentAdjust)
			{
				_generalSettingsRegisterAddr->ExternalSource1CurrentAdjust = currentAdjust;
			}

			/// Включить режим высокой частоты.
			static inline void ExternalClock1HighFreqModeEnable()
			{
				_generalSettingsRegisterAddr->ExternalSource1HighFrequencyModeEnable = 1;
			}

			/// Выключить режим высокой частоты.
			static inline void ExternalClock1HighFreqModeDisable()
			{
				_generalSettingsRegisterAddr->ExternalSource1HighFrequencyModeEnable = 0;
			}

			/// Разрешить использовать внешний тактовый сигнал, поданный на вход XT1.
			static inline void ExternalClock1PinModeEnable()
			{
				_generalSettingsRegisterAddr->ExternalSource1PinSourceEnable = 1;
			}

			/// Запретить использовать внешний тактовый сигнал, поданный на вход XT1.
			static inline void ExternalClock1PinModeDisable()
			{
				_generalSettingsRegisterAddr->ExternalSource1PinSourceEnable = 0;
			}
			
			/// Установить значение внутренней ёмкости для XT1.
			static inline void SetExternalClock1Capasitor(Capasitor capasitor)
			{
				_generalSettingsRegisterAddr->ExternalSource1CapacitorSelect = capasitor;
			}

			/// Включить источник XT2.
			static inline void ExternalClock2On()
			{
				_generalSettingsRegisterAddr->ExternalSource2Off = 0;
			}

			/// Выключить источник XT1.
			static inline void ExternalClock2Off()
			{
				_generalSettingsRegisterAddr->ExternalSource2Off = 1;
			}

			/// Установить уровень энергопотребления источника XT2.
			static inline void SetExternalClock2CurrentAdjust(CurrentAdjust currentAdjust)
			{
				_generalSettingsRegisterAddr->ExternalSource2CurrentAdjust = currentAdjust;
			}

			/// Разрешить использовать внешний тактовый сигнал, поданный на вход XT1.
			static inline void ExternalClock2PinModeEnable()
			{
				_generalSettingsRegisterAddr->ExternalSource2PinSourceEnable = 1;
			}

			/// Разрешить использовать внешний тактовый сигнал, поданный на вход XT2.
			static inline void ExternalClock2PinModeDisable()
			{
				_generalSettingsRegisterAddr->ExternalSource2PinSourceEnable = 0;
			}

			/// Включить SMCLK.
			static inline void PeripheryClockSignalOn()
			{
				_generalSettingsRegisterAddr->PeripheryClockSignalOff = 0;
			}

			/// Выключить SMCLK.
			static inline void PeripheryClockSignalOff()
			{
				_generalSettingsRegisterAddr->PeripheryClockSignalOff = 1;
			}
		};

		/// Доступ к флагам ошибок генераторов (UCSCTL7).
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegister1Address Адрес регистра UCSCTL7.
		template<class RegisterDataType, 
				class PointerType, 
				PointerType ControlRegister1Address>
		class ClockSourceFaultFlagsControl
		{
		protected:

			/// Указатель на регистр UCSCTL7.
			static volatile ClockSourcFaultFlagsRegister * const _faultFlagsRegisterAddr;

		public:

			/// Доступ к регистру UCSCTL7.
			static inline RegisterDataType& FaultFlagsRegister()
			{
				return (RegisterDataType&) *_faultFlagsRegisterAddr;
			}

			/// Проверяет наличие флага ошибки DCO.
			static inline bool InternalSourceFault()
			{
				return _faultFlagsRegisterAddr->InternalSourceFault;
			}

			/// Проверяет наличие флага ошибк XT1 в режиме низкой частоты.
			static inline bool ExternalSource1LowFreqFault()
			{
				return _faultFlagsRegisterAddr->ExternalSource1LowFreqFault;
			}

			/// Проверяет наличие флага ошибк XT1 в режиме высокой частоты.
			static inline bool ExternalSource1HighFreqFault()
			{
				return _faultFlagsRegisterAddr->ExternalSource1HighFreqFault;
			}

			/// Проверяет наличие флага ошибк XT2.
			static inline bool ExternalSource2Fault()
			{
				return _faultFlagsRegisterAddr->ExternalSource2Fault;
			}

			/// Проверяет наличие каких-либо ошибок.
			static inline bool HasFaults()
			{
				return _faultFlagsRegisterAddr->SourceFaults;
			}

			/// Сброс флага ошибки DCO.
			static inline void ClearInternalSourceFault()
			{
				_faultFlagsRegisterAddr->InternalSourceFault = 0;
			}

			/// Сброс флага ошибки XT1 в режиме низкой чатоты.
			static inline void ClearExternalSource1LowFreqFault()
			{
				_faultFlagsRegisterAddr->ExternalSource1LowFreqFault = 0;
			}

			/// Сброс флага ошибки XT1 в режиме высокой чатоты.
			static inline void ClearExternalSource1HighFreqFault()
			{
				_faultFlagsRegisterAddr->ExternalSource1HighFreqFault = 0;
			}

			/// Сброс флага ошибки XT2.
			static inline void ClearExternalSource2Fault()
			{
				_faultFlagsRegisterAddr->ExternalSource2Fault = 0;
			}

			/// Сброс всех флагов ошибок.
			static inline void ClearSourceFaults()
			{
				_faultFlagsRegisterAddr->SourceFaults = 0;
			}

			/// Получить все флаги ошибок.
			static inline RegisterDataType GetSourceFaults()
			{
				return _faultFlagsRegisterAddr->SourceFaults;
			}

		};

		//
		// Инстанцирование статических переменных
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
