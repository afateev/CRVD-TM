
#ifndef POWER_MANAGMENT_MODULE_MSP430F5XXX_H
#define POWER_MANAGMENT_MODULE_MSP430F5XXX_H

#include "PowerManagmentModuleRegisters.h"

namespace Rblib
{
	namespace Msp430f5xxx
	{

		/// ������ ���������� ��������.
		/// \tparam PointerType ��� (������) �������� ������� ���������.
		/// \tparam RegisterAddr ����� �������� PMMCTL0.
		template<class PointerType,
				PointerType RegisterAddr
		>
		class PowerManagmentModule
		{
		protected:

			/// ����� �������� PMMCTL0.
			static volatile PmmControlRegister0* const _pmmControlRegister0Addr;
            /// ������ ��� ������� � ���������.
			static const unsigned char _password = 0xA5;

        protected:

            /// ��������� ������ � �������� PMM.
			static inline void EnableRegistersWriting()
			{
				_pmmControlRegister0Addr->PmmPassword = _password;
			}

            /// ��������� ������ � �������� PMM.
			static inline void DisableRegistersWriting()
			{
				_pmmControlRegister0Addr->PmmPassword = 0;
			}

		public:

			/// ������� ���������� ��� ������� ����.
			enum CoreVoltageLevel
			{
				CoreVoltageLevel0 = 0, ///< ������� 0.
				CoreVoltageLevel1 = 1, ///< ������� 1.
				CoreVoltageLevel2 = 2, ///< ������� 2.
				CoreVoltageLevel3 = 3, ///< ������� 3.
			};

		public:

			/// �������������� ���� �� 4-� ������� ���������� ��� ������� ���� (Vcore).
			/// \details ������� ������� � �������� ��������, �������� ������� ����� �������� �� DVcc.
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
		// ������������� ����������� ����������.
		//

		template<class PointerType, 
			PointerType RegisterAddr>
			volatile PmmControlRegister0* const PowerManagmentModule<PointerType, 
																						RegisterAddr>
			::_pmmControlRegister0Addr = reinterpret_cast<volatile PmmControlRegister0*>(RegisterAddr);

	}
}

#endif // POWER_MANAGMENT_MODULE_MSP430F5XXX_H
