/////////////////////////////////////////////////////////
//
// ��������� ����������� ��������� ������� ������������
//
/////////////////////////////////////////////////////////

#ifndef CLOCK_SYSTEM_REGISTERS_H
#define CLOCK_SYSTEM_REGISTERS_H

namespace Rblib
{
namespace Msp430f5xxx
{
	namespace ClockSystemParts
	{

		/// ������� UCSCTL0.
		union DcoControlRegister1
		{
			struct
			{
				unsigned short Reserved1 : 3;
				/// ����������� ���� DCO � MOD.
				/// \memberof DcoControlRegister1
				unsigned short UnifiedTapField : 10;
				unsigned short Reserved0 : 3;
			};
			struct
			{
				unsigned short Reserved11 : 3;
				/// ���� MOD.
				/// \memberof DcoControlRegister1
				unsigned short ModulationCounter : 5;
				/// ���� DCO.
				/// \memberof DcoControlRegister1
				unsigned short TapSelect : 5;
				unsigned short Reserved00 : 3;
			};
		};

		/// ������� UCSCTL1.
		struct DcoControlRegister2
		{
			/// ���� DISMOD. ���./����. ���������.
			unsigned short ModulationEnable : 1;
			unsigned short Reserved1 : 3;
			/// ���� DCORSEL. ����� ������������ �������� �������.
			unsigned short FrequencyRange : 3;
			unsigned short Reserved0 : 9;
		};

		/// ������� UCSCTL2.
		struct FllControlRegister1
		{
			/// ���� FLLN. ���������� FLL.
			unsigned short Multiplier : 10;
			unsigned short Reserved1 : 2;
			/// ���� FLLD. �������� FLL.
			unsigned short Divider : 3;
			unsigned short Reserved0 : 1;
		};

		/// ������� UCSCTL3.
		struct FllControlRegister2
		{
			/// ���� FLLREFDIV. �������� ��������� �������.
			unsigned short ReferenceDivider : 3;
			unsigned short Reserved1 : 1;
			/// ���� SELREF. �������� ��������� �������.
			unsigned short ReferenceSelect : 3;
			unsigned short Reserved0 : 9;
		};

		/// ������� UCSCLT4.
		struct ClockSourceSelectRegister
		{
			/// ���� SELM. �������� ��� MCLK.
			unsigned short CoreClockSource : 3;
			unsigned short Reserved2 : 1;
			/// ���� SELS. �������� ��� SMCLK.
			unsigned short PeripheryClockSource : 3;
			unsigned short Reserved1 : 1;
			/// ���� SELA. �������� ��� ACLK.
			unsigned short PeripheryAuxClockSource : 3;
			unsigned short Reserved0 : 5;
		};

		/// ������� UCSCLT5.
		struct ClockDividerRegister
		{
			/// ���� DIVM. �������� ��� MCLK.
			unsigned short CoreClockDivider : 3;
			unsigned short Reserved3 : 1;
			/// ���� DIVS. �������� ��� SMCLK.
			unsigned short PeripheryClockDivider : 3;
			unsigned short Reserved2 : 1;
			/// ���� DIVA. �������� ��� ACLK.
			unsigned short PeripheryAuxClockDivider : 3;
			unsigned short Reserved1 : 1;
			/// ���� DIVPA.
			unsigned short PeripheryAuxClockExternalDivider : 3;
			unsigned short Reserved0 : 1;
		};

		/// ������� UCSCLT6.
		struct GeneralSettingsRegister
		{
			/// ���� XT1OFF. ���./����. XT1.
			unsigned short ExternalSource1Off : 1;
			/// ���� SMCLKOFF. ���./����. SMCLK.
			unsigned short PeripheryClockSignalOff : 1;
			/// ���� XCAP. ������� ��� XT1.
			unsigned short ExternalSource1CapacitorSelect : 2;
			/// ���� XT1BYPASS. ���./����. ������� ������ ��� XT1.
			unsigned short ExternalSource1PinSourceEnable : 1;
			/// ���� XTS. ���./����. ����� ������� ������� ��� XT1.
			unsigned short ExternalSource1HighFrequencyModeEnable : 1;
			/// ���� XT1DRIVE. ������������ ������������ ��� ��� XT1.
			unsigned short ExternalSource1CurrentAdjust : 2;
			/// ���� XT2OFF. ���./����. XT2.
			unsigned short ExternalSource2Off : 1;
			unsigned short Reserved1 : 3;
			/// ���� XT2BYPASS. ���./����. ������� ������ ��� XT2.
			unsigned short ExternalSource2PinSourceEnable : 1;
			unsigned short Reserved0 : 1;
			/// ���� XT2DRIVE. ������������ ������������ ��� ��� XT1.
			unsigned short ExternalSource2CurrentAdjust : 2;
		};

		/// ������� UCSCLT7.
		union ClockSourcFaultFlagsRegister
		{
			struct
			{
				/// ���� DOFFG. ���� ������ DCO.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short InternalSourceFault : 1;
				/// ���� ���� XT1LFOFFG. ���� ������ XT1 � ������ ������ �������.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short ExternalSource1LowFreqFault : 1;
				/// ���� XT1HFOFFG. ���� ������ XT1 � ������ ������� �������.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short ExternalSource1HighFreqFault : 1;
				/// ���� XT2OFFG. ���� ������ XT2.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short ExternalSource2Fault : 1;
				unsigned short Reserved0 : 12;
			};
			struct
			{
				/// ����������� ����. ��� ����� ������.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short SourceFaults : 4; // DOFFG, XT1LFOFFG, XT1HFOFFG, XT2OFFG
				unsigned short Reserved00 : 12;
			};
		};

	}
}
}

#endif // CLOCK_SYSTEM_REGISTERS_H
