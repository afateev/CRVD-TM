
#ifndef CLOCK_SYSTEM_CONTROL_MSP430X1XXX_H
#define CLOCK_SYSTEM_CONTROL_MSP430X1XXX_H

#include "../x1xxx,x2xxx/ClockSystemControl.h"

namespace Rblib
{
namespace Msp430x1xxx
{
	namespace ClockSystemParts
	{
		/// ����� ���������� ��������� DCOCTL 
		/// � ����������� DCO �� ������ ����������� ��������� (BCSCTL1 � BCSCTL2).
		/// \tparam RegisterDataType ��� ������ (������) ��������.
		/// \tparam PointerType ��� ��������� �� �������.
		/// \tparam DcoControlRegisterAddress ����� �������� DCOCTL.
		/// \tparam ControlRegisterAddress1 ����� �������� BCSCTL1.
		/// \tparam ControlRegisterAddress2 ����� �������� BCSCTL2.
		template<class RegisterDataType,
				class PointerType,
				PointerType DcoControlRegisterAddress,
				PointerType ControlRegister1Address, 
				PointerType ControlRegister2Address>
    class DcoControl : public Msp430x12xxx::ClockSystemParts::DcoControl<RegisterDataType, PointerType, DcoControlRegisterAddress, ControlRegister1Address, ControlRegister2Address>
		{
		public:

			/// ���������� ������������ �������� ��������� ������� (���� RSELx).
			static inline unsigned char MaxFrequencyRange()
			{
  			return 0x07;
			}

		};

	}
}
}

#endif // CLOCK_SYSTEM_CONTROL_MSP430X1XXX_H
