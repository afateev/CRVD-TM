
#ifndef CLOCK_SYSTEM_REGISTERS_MSP430X1XXX_MSP430X2XXX_H
#define CLOCK_SYSTEM_REGISTERS_MSP430X1XXX_MSP430X2XXX_H

namespace Rblib
{
namespace Msp430x12xxx
{
	namespace ClockSystemParts
	{
		/// ��������� �������� DCOCTL
		union DcoControlRegister
		{
			struct
			{
				/// ���� MODx.
				/// \memberof DcoControlRegister
				unsigned char ModulatorSelect : 5; // MODx
				/// ���� DCOx.
				/// \memberof DcoControlRegister
				unsigned char FrequencySelect : 3; // DCOx
			};
			struct
			{
				/// ����������� ���� DCOx + MODx.
				/// \memberof DcoControlRegister
				unsigned char UnifiedTapField : 8; // DCOx + MODx
			};
		};

		/// ��������� �������� BCSCTL1.
		struct ControlRegister1
		{
			/// ���� RSELx.
			unsigned char FrequencyRange : 4;
			/// ���� DIVAx.
			unsigned char PeripheryAuxClockDivider : 2;
			/// ���� XTS.
			unsigned char ExternalSource1HighFrequencyModeEnable : 1;
			/// ���� XT2OFF.
			unsigned char ExternalSource2Off : 1;
		};

		/// ��������� �������� BCSCTL2.
		struct ControlRegister2
		{
			/// ���� DCOR.
			unsigned char InternalSourceResistor : 1;
			/// ���� DIVSx.
			unsigned char PeripheryClockDivider : 2;
			/// ���� SELS.
			unsigned char PeripheryClockSource : 1;
			/// ���� DIVMx.
			unsigned char CoreClockDivider : 2;
			/// ���� SELMx.
			unsigned char CoreClockSource : 2;
		};

	}
}
}

#endif // CLOCK_SYSTEM_REGISTERS_MSP430X1XXX_MSP430X2XXX_H
