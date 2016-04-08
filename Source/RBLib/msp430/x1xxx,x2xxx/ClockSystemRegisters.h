
#ifndef CLOCK_SYSTEM_REGISTERS_MSP430X1XXX_MSP430X2XXX_H
#define CLOCK_SYSTEM_REGISTERS_MSP430X1XXX_MSP430X2XXX_H

namespace Rblib
{
namespace Msp430x12xxx
{
	namespace ClockSystemParts
	{
		/// Структура регистра DCOCTL
		union DcoControlRegister
		{
			struct
			{
				/// Поле MODx.
				/// \memberof DcoControlRegister
				unsigned char ModulatorSelect : 5; // MODx
				/// Поле DCOx.
				/// \memberof DcoControlRegister
				unsigned char FrequencySelect : 3; // DCOx
			};
			struct
			{
				/// Объединённое поле DCOx + MODx.
				/// \memberof DcoControlRegister
				unsigned char UnifiedTapField : 8; // DCOx + MODx
			};
		};

		/// Структура регистра BCSCTL1.
		struct ControlRegister1
		{
			/// Поле RSELx.
			unsigned char FrequencyRange : 4;
			/// Поле DIVAx.
			unsigned char PeripheryAuxClockDivider : 2;
			/// Поле XTS.
			unsigned char ExternalSource1HighFrequencyModeEnable : 1;
			/// Поле XT2OFF.
			unsigned char ExternalSource2Off : 1;
		};

		/// Структура регистра BCSCTL2.
		struct ControlRegister2
		{
			/// Поле DCOR.
			unsigned char InternalSourceResistor : 1;
			/// Поле DIVSx.
			unsigned char PeripheryClockDivider : 2;
			/// Поле SELS.
			unsigned char PeripheryClockSource : 1;
			/// Поле DIVMx.
			unsigned char CoreClockDivider : 2;
			/// Поле SELMx.
			unsigned char CoreClockSource : 2;
		};

	}
}
}

#endif // CLOCK_SYSTEM_REGISTERS_MSP430X1XXX_MSP430X2XXX_H
