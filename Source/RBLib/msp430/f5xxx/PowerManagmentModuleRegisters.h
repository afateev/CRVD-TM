
#ifndef POWER_MANAGMENT_MODULE_REGISTERS_MSP430F5XXX_H
#define POWER_MANAGMENT_MODULE_REGISTERS_MSP430F5XXX_H

namespace Rblib
{
	namespace Msp430f5xxx
	{

		/// ��������� �������� PMMCTL0.
		struct PmmControlRegister0
		{
			/// ���� PMMCOREV.
			unsigned char CoreVoltageLevel : 2;
			/// ���� PMMSWBOR.
			unsigned char  SoftwareBrownoutReset : 1;
			/// ���� PMMSWPOR.
			unsigned char  SoftwarePowerOnReset : 1;
			/// ���� PMMREGOFF.
			unsigned char  RegulatorOff : 1;
			unsigned char  Reserved0 : 2;
			unsigned char  Reserved1 : 1;
			/// ���� PMMPW
			unsigned char  PmmPassword : 8;
		};

	}
}

#endif // POWER_MANAGMENT_MODULE_REGISTERS_MSP430F5XXX_H
