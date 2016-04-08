
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
		/// Класс управления регистром BCSCTL3.
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegisterAddress Адрес регистра BCSCTL3.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegisterAddress>
		class Control3
		{
		protected:

			/// Указатель нарегистр BCSCTL3.
			static volatile ControlRegister3* const _controlRegisterAddr;

		public:

			/// Значения ёмкостей.
			enum Capasitor
			{
				/// 1пФ.
				Capasitor1Pf = 0,
				/// 6пФ.
				Capasitor6Pf = 1,
				/// 10пФ.
				Capasitor10Pf = 2,
				/// 12.5пФ.
				Capasitor12Pf = 3
			};

			/// Режимы работы для XT1 и XT2.
			enum ExternlaSourceMode
			{
				/// Частоты 0.4 - 1 MHz в режиме HF (XT1/XT2) или 32768Hz в режиме LF (XT1).
				ExternalSourceMode1 = 0,
				/// Для частот 1 - 3 MHz в режимме HF (XT2).
				ExternalSourceMode2 = 1,
				/// Для частот 3 - 16 MHz в режиме HF (XT1/XT2) или VLOCLK в режиме LF(XT1).
				ExternalSourceMode3 = 2,
				/// Внешний тактовый сигнал на ноге для обоих режимов (HF и LF) обоих источников (XT1/XT2).
				ExternalSourceMode4 = 3,
			};

		public:

			/// Доступ к регистру BCSCTL3.
			static inline RegisterDataType& GetControlRegister3()
			{
				return (RegisterDataType&) *_controlRegisterAddr;
			}

			/// Задать значение ёмкости для XT1 в режиме низкой частоты (XCAPx).
			static inline void SetExternalClock1Capasitor(Capasitor value)
			{
				_controlRegisterAddr->ExternalSource1Capacitor = value;
			}

			/// Установить режим работы XT1 (LFXT1Sx).
			static inline void SetExternalClock1Mode(ExternlaSourceMode value)
			{
				_controlRegisterAddr->ExternalSource1Settings = value;
			}

			/// Возвращает режим работы режим работы XT1 (LFXT1Sx).
			static inline ExternlaSourceMode GetExternalClock1Mode()
			{
				return (ExternlaSourceMode) _controlRegisterAddr->ExternalSource1Settings;
			}

			/// Установить режим работы XT2 (XT2Sx).
			static inline void SetExternalClock2Mode(ExternlaSourceMode value)
			{
				_controlRegisterAddr->ExternalSource2Settings = value;
			}

			/// Возвращает режим работы режим работы XT2 (XT2Sx).
			static inline ExternlaSourceMode GetExternalClock2Mode()
			{
				return (ExternlaSourceMode) _controlRegisterAddr->ExternalSource2Settings;
			}

			/// Проверить наличие ошибки осциллятора XT1 (LFXT1OF).
			/// \return @c true, если ошибка есть, или @c false, если ошибки нет.
			static inline bool GetExternalClock1Fault()
			{
				return _controlRegisterAddr->ExternalSource1Fault;
			}

			/// Проверить наличие ошибки осциллятора XT2 (XT2OF).
			/// \return @c true, если ошибка есть, или @c false, если ошибки нет.
			static inline bool GetExternalClock2Fault()
			{
				return _controlRegisterAddr->ExternalSource2Fault;
			}

			/// Сбросить флаг ошибки осциллятора XT1 (LFXT1OF).
			static inline void ClearExternalClock1Fault()
			{
				_controlRegisterAddr->ExternalSource1Fault = 0;
			}

			/// Сбросить флаг ошибки осциллятора XT2 (XT2OF).
			static inline void ClearExternalClock2Fault()
			{
				_controlRegisterAddr->ExternalSource2Fault = 0;
			}

			/// Проверить наличие ошибок осцилляторов XT1 и XT2 (LFXT1OF,XT2OF).
			/// \return @c true, если ошибки есть, или @c false, если ошибок нет.
			static inline bool GetExternalClockFaults()
			{
				return _controlRegisterAddr->ExternalSourceFaults;
			}

			/// Сбросить флаги ошибок (LFXT1OF,XT2OF).
			static inline void ClearExternalClockFaults()
			{
				_controlRegisterAddr->ExternalSourceFaults = 0;
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
				PointerType ControlRegister2Address, 
				PointerType ControlRegister3Address>
    class ClockSourceBindControl : public Msp430x12xxx::ClockSystemParts::ClockSourceBindControl<RegisterDataType, PointerType, ControlRegister1Address, ControlRegister2Address>
		{
		protected:

      typedef Msp430x12xxx::ClockSystemParts::ClockSourceBindControl<RegisterDataType, PointerType, ControlRegister1Address, ControlRegister2Address> _Base;
			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		public:

			/// Источники тактового сигнала.
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

			/// Доступ к регистру BCSCTL3.
			static inline RegisterDataType& GetControlRegister3()
			{
				return _Control3::GetControlRegister3();
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
					// проверяем, не находится ли XT1 в режиме VLO
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
						return false;
					_Base::_Control2::SetCoreSignalSource(_Base::_Control2::CoreSignalSourceExternal1);
					return true;
				case ClockSourceInternalLowFreq1:
					// VLO в данном семействе - режим XT1
					// проверяем, в нужном ли режиме XT1
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
					{
						_Base::_Control2::SetCoreSignalSource(_Base::_Control2::CoreSignalSourceExternal1);
						return true;
					}
				}

        return _Base::SetCoreSignalSource((typename _Base::ClockSource)source);
			}

      /// Задать источник SMCLK.
      /// \param source Источник такового сигнала.
      /// return @c true - выполнено успешно. @c false - не удалось назначить источник (текущие настройки не позволяют использовать заданный источник, либо SMCLK не может работать с заданным источником).
      static inline bool SetPeripherySignalSource(ClockSource source)
      {
        return _Base::SetPeripherySignalSource((typename _Base::ClockSource)source);
      }

			/// Задать источник ACLK.
			/// \param source Источник такового сигнала.
			/// return @c true - выполнено успешно. @c false - не удалось назначить источник (текущие настройки не позволяют использовать заданный источник, либо ACLK не может работать с заданным источником).
			static inline bool SetPeripheryAuxSignalSource(ClockSource source)
			{
				switch(source)
				{
				case ClockSourceExternal1:
					// проверяем, не находится ли XT1 в режиме VLO
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
						return false;
					return true;
				case ClockSourceInternalLowFreq1:
					// VLO в данном семействе - режим XT1
					// проверяем, в нужном ли режиме XT1
					if (_Base::_Control1::ExternalClock1HighFreqModeDisabled() 
						&& _Control3::GetExternalClock1Mode() == _Control3::ExternalSourceMode3)
						return true;
					return false;
				}
        return false;
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
    class DcoControl : public Msp430x12xxx::ClockSystemParts::DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address>
		{
        protected:
            typedef Msp430x12xxx::ClockSystemParts::DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address> _Base;
		public:

			/// Возвращает максимальное значение диапазона частоты (поля RSELx).
			static inline unsigned char MaxFrequencyRange()
			{
				if (_Base::ResistorExternal())
					return 0x07;
				else
					return 0x0F;
			}

		};

		/// Класс управления флагами ошибок осцилляторов (BCSCTL3).
		/// \tparam RegisterDataType Тип данных (размер) регистра.
		/// \tparam PointerType Тип указателя на регистр.
		/// \tparam ControlRegisterAddress3 Адрес регистра BCSCTL3.
		template<class RegisterDataType,
				class PointerType,
				PointerType ControlRegister3Address>
		class ClockSourceFaultFlagsControl
		{
		protected:

			typedef Control3<RegisterDataType, PointerType, ControlRegister3Address> _Control3;

		public:

			/// Доступ к регистру BCSCTL3.
			static inline RegisterDataType& GetControlRegister3()
			{
				return _Control3::GetControlRegister3();
			}

			/// Проверяет наличие ошибки источника XT1 (LFXT1OF).
			/// \return @c true - есть ошибка. @c false - нет ошибки.
			static inline bool ExternalSource1Fault()
			{
				return _Control3::GetExternalClock1Fault();
			}

			/// Проверяет наличие ошибки источника XT2 (XT2OF).
			/// \return @c true - есть ошибка. @c false - нет ошибки.
			static inline bool ExternalSource2Fault()
			{
				return _Control3::GetExternalClock2Fault();
			}

			/// Сбрасывает флаги ошибок истоника XT1 (LFXT1OF).
			static inline void ClearExternalSource1Fault()
			{
				_Control3::ClearExternalClock1Fault();
			}

			/// Сбрасывает флаги ошибок истоника XT2 (XT2OF).
			static inline void ClearExternalSource2Fault()
			{
				_Control3::ClearExternalClock2Fault();
			}

			/// Проверяет наличие ошибок источников XT1/XT2 (LFXT1OF, XT2OF).
			/// \return @c true - есть ошибки. @c false - нет ошибок.
			static inline bool HasFaults()
			{
				return _Control3::GetExternalClockFaults();
			}
			
			/// Сбросить флаги ошибок осцилляторов (LFXT1OF, XT2OF).
			static inline void ClearSourceFaults()
			{
				_Control3::ClearExternalClockFaults();
			}

			/// Возвращает значение объединённого поля LFXT1OF + XT2OF
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
