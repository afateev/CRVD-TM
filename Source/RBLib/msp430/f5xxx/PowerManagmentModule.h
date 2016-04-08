
#ifndef POWER_MANAGMENT_MODULE_MSP430F5XXX_H
#define POWER_MANAGMENT_MODULE_MSP430F5XXX_H

#include "PowerManagmentModuleRegisters.h"

namespace Rblib
{
	namespace Msp430f5xxx
	{

		/// Модуль управления питанием.
		/// \tparam PointerType Тип (размер) значения адресов регистров.
		/// \tparam RegisterAddr Адрес регистра PMMCTL0.
		template<class PointerType,
				PointerType RegisterAddr
		>
		class PowerManagmentModule
		{
		protected:

			/// Адрес регистра PMMCTL0.
			static volatile PmmControlRegister0* const _pmmControlRegister0Addr;
            /// пароль для доступа к регистрам.
			static const unsigned char _password = 0xA5;

        protected:

            /// Разрешает запись в регистры PMM.
			static inline void EnableRegistersWriting()
			{
				_pmmControlRegister0Addr->PmmPassword = _password;
			}

            /// Запрещает запись в регистры PMM.
			static inline void DisableRegistersWriting()
			{
				_pmmControlRegister0Addr->PmmPassword = 0;
			}

		public:

			/// Уровень напряжения для питания ядра.
			enum CoreVoltageLevel
			{
				CoreVoltageLevel0 = 0, ///< Уровень 0.
				CoreVoltageLevel1 = 1, ///< Уровень 1.
				CoreVoltageLevel2 = 2, ///< Уровень 2.
				CoreVoltageLevel3 = 3, ///< Уровень 3.
			};

		public:

			/// Устанавлиевает один из 4-х уровней напряжения для питания ядра (Vcore).
			/// \details Уровень задаётся в условных еденицах, реальный уровень будет зависеть от DVcc.
			static inline void SetCoreVoltageLevel(CoreVoltageLevel voltageLevel)
			{
				EnableRegistersWriting();
				_pmmControlRegister0Addr->CoreVoltageLevel = voltageLevel;
				DisableRegistersWriting();
			}
      
      static inline void GeneratePOR()
      {
        _pmmControlRegister0Addr->SoftwarePowerOnReset = 1;
      }

			static inline void RegulatorOn()
			{
				EnableRegistersWriting();
				_pmmControlRegister0Addr->RegulatorOff = 0;
				DisableRegistersWriting();
			}

			static inline void RegulatorOff()
			{
				EnableRegistersWriting();
				_pmmControlRegister0Addr->RegulatorOff = 1;
				DisableRegistersWriting();
			}
		};

		//
		// Инициализация статических переменных.
		//

		template<class PointerType, 
			PointerType RegisterAddr>
			volatile PmmControlRegister0* const PowerManagmentModule<PointerType, 
																						RegisterAddr>
			::_pmmControlRegister0Addr = reinterpret_cast<volatile PmmControlRegister0*>(RegisterAddr);

	}
}

#endif // POWER_MANAGMENT_MODULE_MSP430F5XXX_H
