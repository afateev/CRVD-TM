
#ifndef CLOCK_SYSTEM_CONTROL_MSP430X1XXX_MSP430X2XXX_H
#define CLOCK_SYSTEM_CONTROL_MSP430X1XXX_MSP430X2XXX_H

#include "ClockSystemRegisters.h"

namespace Rblib
{
namespace Msp430x12xxx
{
	namespace ClockSystemParts
	{

		/// Класс для работы с набором констант-делителей
		class SourceDivider
		{
		public:

			/// Делители.
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

			/// Преобразовать делитель из числа в енам, округляя до меньшего.
			/// \param divider Делитель-число.
			/// \return Делитель-енам.
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

			/// Преобразовать делитель из числа в енам, округляя до большего.
			/// \param divider Делитель-число.
			/// \return Делитель-енам.
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

			/// Преобразовать делитель из енама в число.
			/// \param divider Делитель-енам.
			/// \return Делитель-число.
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

		/// Класс управления регистром BCSCTL1.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegisterAddress Адрес регистра BCSCTL1.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegisterAddress>
		class Control1 : virtual public SourceDivider
		{
		protected:

			/// Указатель на регистр BCSCTL1.
			static volatile ControlRegister1* const _controlRegisterAddr;

		public:

			/// Доступ к регистру BCSCTL1.
			static inline RegisterDataType& GetControlRegister1()
			{
				return (RegisterDataType&) *_controlRegisterAddr;
			}

			/// Задать диапазон частоты (RSELx).
			/// \param value Диапазон частоты 0..7, если используется внешний резистор (DCOR=1), или 0..15, если используется внутренний резистор (DCOR=0).
			static inline void SetInternalClockFrequencyRange(RegisterDataType value)
			{
				// учитываем баг просессора (slaz033b.pdf, BCL12)
				if (_controlRegisterAddr->FrequencyRange > 13 && value < 13)
				{
					_controlRegisterAddr->FrequencyRange = 13;
				}
				_controlRegisterAddr->FrequencyRange = value;
			}

			/// Возвращает выбранный диапазон чатоты (RSELx).
			static inline RegisterDataType GetInternalClockFrequencyRange()
			{
				return _controlRegisterAddr->FrequencyRange;
			}

			/// Увеличить на 1 значение поля RSELx.
			static inline void IncInternalClockFrequencyRange()
			{
				_controlRegisterAddr->FrequencyRange++;
			}

			/// Уменьшить на 1 значение поля RSELx.
			static inline void DecInternalClockFrequencyRange()
			{
				_controlRegisterAddr->FrequencyRange--;
			}

			/// Задать делитель ACLK (DIVAx).
			static inline void SetPeripheryAuxSignalDivider(Divider value)
			{
				_controlRegisterAddr->PeripheryAuxClockDivider = value;
			}

			/// Управление режимом высокой чатоты XT1 (XTS).
			/// \param highFreqMode Значение поля XTS.
			static inline void SetExternalClock1HighFreqMode(bool highFreqMode)
			{
				_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable = highFreqMode;
			}

			/// Включить режим высокой частоты XT1 (XTS=1).
			static inline void ExternalClock1HighFreqModeEnable()
			{
				_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable = 1;
			}

			/// Выключить режим высокой частоты XT1 (XTS=0).
			static inline void ExternalClock1HighFreqModeDisable()
			{
				_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable = 0;
			}

			/// Проверить включен ли режим высокой чатоты XT1.
			/// return @c true, если режим ВЧ включен, или @c false, если режим ВЧ выкчлючен.
			static inline bool ExternalClock1HighFreqModeEnabled()
			{
				return _controlRegisterAddr->ExternalSource1HighFrequencyModeEnable;
			}

			/// Проверить выключен ли режим высокой чатоты XT1.
			/// return @c true, если режим ВЧ выключен, или @c false, если режим ВЧ вкчлючен.
			static inline bool ExternalClock1HighFreqModeDisabled()
			{
				return (_controlRegisterAddr->ExternalSource1HighFrequencyModeEnable == 0);
			}

			/// Включить XT2.
			static inline void ExternalClock2Enable()
			{
				_controlRegisterAddr->ExternalSource2Off = 0;
			}

			/// Выключить XT2.
			/// \details Выключает XT2, если он не используется в качестве источнка для MCLK или SMCLK.
			static inline void ExternalClock2Disable()
			{
				_controlRegisterAddr->ExternalSource2Off = 1;
			}

		};

		/// Класс управления регистром BCSCTL2.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegisterAddress Адрес регистра BCSCTL2.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegisterAddress>
		class Control2 : virtual public SourceDivider
		{
		protected:

			/// Указатель на регистр BCSCTL2.
			static volatile ControlRegister2* const _controlRegisterAddr;

		public:

			/// Источники для MCLK.
			enum CoreSignalSource
			{
				/// DCO.
				CoreSignalSourceInternal = 0x00,
				/// XT1.
				CoreSignalSourceExternal1 = 0x11,
				/// XT2.
				CoreSignalSourceExternal2 = 0x10
			};

			/// Источники для MCLK.
			enum PeripherySignalSource
			{
				/// DCO.
				PeripherySignalSourceInternal = 0,
				/// XT2, если он есть, XT1/VLOCLK, если нету XT2.
				PeripherySignalSourceExternal = 1
			};

		public:

			/// Доступ к регистру BCSCTL2.
			static inline RegisterDataType& GetControlRegister2()
			{
				return (RegisterDataType&) *_controlRegisterAddr;
			}

			/// Установить делитель для SMCLK (DIVSx).
			static inline void SetPeripherySignalDivider(Divider value)
			{
				_controlRegisterAddr->PeripheryClockDivider = value;
			}

			/// Установить делитель для MCLK (DIVMx).
			static inline void SetCoreSignalDivider(Divider value)
			{
				_controlRegisterAddr->CoreClockDivider = value;
			}

			/// Задать источник для SMCLK (SELS).
			static inline void SetPeripherySignalSource(PeripherySignalSource value)
			{
				_controlRegisterAddr->PeripheryClockSource = value;
			}

			/// Задать источник для MCLK (SELMx).
			static inline void SetCoreSignalSource(CoreSignalSource value)
			{
				_controlRegisterAddr->CoreClockSource = value;
			}

			/// Использовать внутренний резистор (DCOR=0).
			static inline void SetInternalSourceResistorInternal()
			{
				_controlRegisterAddr->InternalSourceResistor = 0;
			}

			/// Использовать внешний резистор (DCOR=1).
			static inline void SetInternalSourceResistorExternal()
			{
				_controlRegisterAddr->InternalSourceResistor = 1;
			}

			/// Проверить какой резистор испольщуется.
			/// return @c true, если используется внешний резистор (DCOR=1), или @c false, если испольщуется внутренний резистор (DCOR=0).
			static inline bool InternalSourceResistorExternal()
			{
				return _controlRegisterAddr->InternalSourceResistor;
			}

		};

		/// Обобщённый класс для управления привязкой источников к сигналам и делителями.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegisterAddress1 Адрес регистра BCSCTL1.
		/// \tparam ControlRegisterAddress2 Адрес регистра BCSCTL2.
		/// \tparam ControlRegisterAddress3 Адрес регистра BCSCTL3.
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

			/// Источники тактового сигнала.
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

			/// Доступ к регистру BCSCTL1.
			static inline RegisterDataType& GetControlRegister1()
			{
				return _Control1::GetControlRegister1();
			}

			/// Доступ к регистру BCSCTL2.
			static inline RegisterDataType& GetControlRegister2()
			{
				return _Control2::GetControlRegister2();
			}

			// источники

			/// Задать источник MCLK.
			/// \param source Источник такового сигнала.
			/// return @c true - выполнено успешно. @c false - не удалось назначить источник (текущие настройки не позволяют использовать заданный источник).
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

			/// Задать источник SMCLK.
			/// \param source Источник такового сигнала.
			/// return @c true - выполнено успешно. @c false - не удалось назначить источник (текущие настройки не позволяют использовать заданный источник, либо SMCLK не может работать с заданным источником).
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

			/// Задать источник ACLK.
			/// \param source Источник такового сигнала.
			/// return @c true - выполнено успешно. @c false - не удалось назначить источник (текущие настройки не позволяют использовать заданный источник, либо ACLK не может работать с заданным источником).
			static inline bool SetPeripheryAuxSignalSource(ClockSource source)
			{
				switch(source)
				{
				case ClockSourceExternal1:
					return true;
				}

				return false;
			}

			// делители

			/// Установить делитель MCLK (DIVMx/BCSCTL2).
			static inline void SetCoreSignalDivider(Divider divider)
			{
				_Control2::SetCoreSignalDivider(divider);
			}

			/// Установить делитель SMCLK (DIVSx)/BCSCTL2.
			static inline void SetPeripherySignalDivider(Divider divider)
			{
				_Control2::SetPeripherySignalDivider(divider);
			}

			/// Установить делитель ACLK (DIVAx/BCSCTL1).
			static inline void SetPeripheryAuxSignalDivider(Divider divider)
			{
				_Control1::SetPeripheryAuxSignalDivider(divider);
			}

			/// Установить делитель MCLK (DIVMx/BCSCTL2).
			static inline void SetCoreSignalDivider(unsigned long divider)
			{
				_Control2::SetCoreSignalDivider(GetDivider(divider));
			}

			/// Установить делитель SMCLK (DIVSx/BCSCTL2).
			static inline void SetPeripherySignalDivider(unsigned long divider)
			{
				_Control2::SetPeripherySignalDivider(GetDivider(divider));
			}

			/// Установить делитель ACLK (DIVAx/BCSCTL1).
			static inline void SetPeripheryAuxSignalDivider(unsigned long divider)
			{
				_Control1::SetPeripheryAuxSignalDivider(GetDivider(divider));
			}
		};

		/// Класс управления регистром DCOCTL 
		/// и настройками DCO из других управляющих регистров (BCSCTL1 и BCSCTL2).
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam DcoControlRegisterAddress Адрес регистра DCOCTL.
		/// \tparam ControlRegisterAddress1 Адрес регистра BCSCTL1.
		/// \tparam ControlRegisterAddress2 Адрес регистра BCSCTL2.
		template<class RegisterDataType,
				class PointerType,
				PointerType DcoControlRegisterAddress,
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
		class DcoControl
		{

		protected:

			/// Указатель на регистр DCOCTL
			static volatile DcoControlRegister* const _dcoControlRegisterAddr;

		protected:

			typedef Control1<RegisterDataType, PointerType, ControlRegister1Address> _Control1;
			typedef Control2<RegisterDataType, PointerType, ControlRegister2Address> _Control2;

		public:

			/// Константы.
			enum Constants
			{
				/// Максимальное значение объединённого поля DCO + MOD регистра DCOCTL.
				MaxUnifiedTapSelect = 0xFF,
			};

		public:

			/// Доступ к регистру DCOCTL.
			static inline RegisterDataType& GetDcoControlRegister()
			{
				return (RegisterDataType&) *_dcoControlRegisterAddr;
			}

			/// Доступ к регистру BCSCTL1.
			static inline RegisterDataType& GetControlRegister1()
			{
				return _Control1::GetControlRegister1();
			}

			/// Доступ к регистру BCSCTL2.
			static inline RegisterDataType& GetControlRegister2()
			{
				return _Control2::GetControlRegister2();
			}

			static inline void EnableModulation()
			{
			}

			/// Устанавливает значение поля MOD регистра DCOCTL.
			static inline void SetModulationCounter(RegisterDataType modulatorSelect)
			{
				_dcoControlRegisterAddr->ModulatorSelect = modulatorSelect;
			}

			/// Возвращает значение поля MOD регистра DCOCTL.
			static inline RegisterDataType GetModulationCounter()
			{
				return _dcoControlRegisterAddr->ModulatorSelect;
			}

			/// Устанавливает значение поля DCO регистра DCOCTL.
			static inline void SetTapSelect(RegisterDataType frequencySelect)
			{
				_dcoControlRegisterAddr->FrequencySelect = frequencySelect;
			}

			/// Возвращает значение поля DCO регистра DCOCTL.
			static inline RegisterDataType GetTapSelect()
			{
				return _dcoControlRegisterAddr->FrequencySelect;
			}

			/// Устанавливает значение объединённого поля DCO + MOD регистра DCOCTL.
			static inline void SetUnifiedTapSelect(RegisterDataType tapField)
			{
				_dcoControlRegisterAddr->UnifiedTapField = tapField;
			}

			/// Возвращает значение объединённого поля DCO + MOD регистра DCOCTL.
			static inline RegisterDataType GetUnifiedTapSelect()
			{
				return _dcoControlRegisterAddr->UnifiedTapField;
			}

			/// Увеличивает на 1 значение объединённого поля DCO + MOD регистра DCOCTL.
			static inline void IncUnifiedTapSelect()
			{
				_dcoControlRegisterAddr->UnifiedTapField++;
			}

			/// Уменьшает на 1 значение объединённого поля DCO + MOD регистра DCOCTL.
			static inline void DecUnifiedTapSelect()
			{
				_dcoControlRegisterAddr->UnifiedTapField--;
			}

			/// Возвращает диапазон частоты. Поле RSELx регистра BCSCTL1.
			static inline RegisterDataType GetFrequencyRange()
			{
				return _Control1::GetInternalClockFrequencyRange();
			}

			/// Устанавливает диапазон частоты. Поле RSELx регистра BCSCTL1.
			static inline void SetFrequencyRange(RegisterDataType frequencyRange)
			{
				_Control1::SetInternalClockFrequencyRange(frequencyRange);
			}

			/// Увеличивает на 1 значение поле RSELx регистра BCSCTL1.
			static inline void IncFrequencyRange()
			{
				_Control1::IncInternalClockFrequencyRange();
			}

			/// Уменьшает на 1 значение поле RSELx регистра BCSCTL1.
			static inline void DecFrequencyRange()
			{
				_Control1::DecInternalClockFrequencyRange();
			}

			/// Использовать внутренний резистор.
			static inline void SetResistorInternal()
			{
				_Control2::SetInternalSourceResistorInternal();
			}

			/// Использовать внешний резистор.
			static inline void SetResistorExternal()
			{
				_Control2::SetInternalSourceResistorExternal();
			}

			/// Проверяет используется ли внешний резистор.
			/// return @c true - внешний резистор. @c false - внутренний резистор.
			static inline bool ResistorExternal()
			{
				return _Control2::InternalSourceResistorExternal();
			}

		};

		//
		// Инстанцирование статических переменных
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
