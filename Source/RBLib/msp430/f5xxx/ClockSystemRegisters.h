/////////////////////////////////////////////////////////
//
// Структура управляющих регистров системы тактирования
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

		/// Регистр UCSCTL0.
		union DcoControlRegister1
		{
			struct
			{
				unsigned short Reserved1 : 3;
				/// Объединённые поля DCO и MOD.
				/// \memberof DcoControlRegister1
				unsigned short UnifiedTapField : 10;
				unsigned short Reserved0 : 3;
			};
			struct
			{
				unsigned short Reserved11 : 3;
				/// Поле MOD.
				/// \memberof DcoControlRegister1
				unsigned short ModulationCounter : 5;
				/// Поле DCO.
				/// \memberof DcoControlRegister1
				unsigned short TapSelect : 5;
				unsigned short Reserved00 : 3;
			};
		};

		/// Регистр UCSCTL1.
		struct DcoControlRegister2
		{
			/// Поле DISMOD. Вкл./Выкл. модуляцию.
			unsigned short ModulationEnable : 1;
			unsigned short Reserved1 : 3;
			/// Поле DCORSEL. Задаёт используемый диапазон частоты.
			unsigned short FrequencyRange : 3;
			unsigned short Reserved0 : 9;
		};

		/// Регистр UCSCTL2.
		struct FllControlRegister1
		{
			/// Поле FLLN. Множителдь FLL.
			unsigned short Multiplier : 10;
			unsigned short Reserved1 : 2;
			/// Поле FLLD. Делитель FLL.
			unsigned short Divider : 3;
			unsigned short Reserved0 : 1;
		};

		/// Регистр UCSCTL3.
		struct FllControlRegister2
		{
			/// Поле FLLREFDIV. Делитель эталонной частоты.
			unsigned short ReferenceDivider : 3;
			unsigned short Reserved1 : 1;
			/// Поле SELREF. Источник эталонной частоты.
			unsigned short ReferenceSelect : 3;
			unsigned short Reserved0 : 9;
		};

		/// Регистр UCSCLT4.
		struct ClockSourceSelectRegister
		{
			/// Поле SELM. Источник для MCLK.
			unsigned short CoreClockSource : 3;
			unsigned short Reserved2 : 1;
			/// Поле SELS. Источник для SMCLK.
			unsigned short PeripheryClockSource : 3;
			unsigned short Reserved1 : 1;
			/// Поле SELA. Источник для ACLK.
			unsigned short PeripheryAuxClockSource : 3;
			unsigned short Reserved0 : 5;
		};

		/// Регистр UCSCLT5.
		struct ClockDividerRegister
		{
			/// Поле DIVM. Делитель для MCLK.
			unsigned short CoreClockDivider : 3;
			unsigned short Reserved3 : 1;
			/// Поле DIVS. Делитель для SMCLK.
			unsigned short PeripheryClockDivider : 3;
			unsigned short Reserved2 : 1;
			/// Поле DIVA. Делитель для ACLK.
			unsigned short PeripheryAuxClockDivider : 3;
			unsigned short Reserved1 : 1;
			/// Поле DIVPA.
			unsigned short PeripheryAuxClockExternalDivider : 3;
			unsigned short Reserved0 : 1;
		};

		/// Регистр UCSCLT6.
		struct GeneralSettingsRegister
		{
			/// Поле XT1OFF. Вкл./Выкл. XT1.
			unsigned short ExternalSource1Off : 1;
			/// Поле SMCLKOFF. Вкл./Выкл. SMCLK.
			unsigned short PeripheryClockSignalOff : 1;
			/// Поле XCAP. Ёмкость для XT1.
			unsigned short ExternalSource1CapacitorSelect : 2;
			/// Поле XT1BYPASS. Вкл./Выкл. внешний сигнал для XT1.
			unsigned short ExternalSource1PinSourceEnable : 1;
			/// Поле XTS. Вкл./Выкл. режим высокой частоты для XT1.
			unsigned short ExternalSource1HighFrequencyModeEnable : 1;
			/// Поле XT1DRIVE. Максимальный потребляемый ток для XT1.
			unsigned short ExternalSource1CurrentAdjust : 2;
			/// Поле XT2OFF. Вкл./Выкл. XT2.
			unsigned short ExternalSource2Off : 1;
			unsigned short Reserved1 : 3;
			/// Поле XT2BYPASS. Вкл./Выкл. внешний сигнал для XT2.
			unsigned short ExternalSource2PinSourceEnable : 1;
			unsigned short Reserved0 : 1;
			/// Поле XT2DRIVE. Максимальный потребляемый ток для XT1.
			unsigned short ExternalSource2CurrentAdjust : 2;
		};

		/// Регистр UCSCLT7.
		union ClockSourcFaultFlagsRegister
		{
			struct
			{
				/// Поле DOFFG. Флаг ошибки DCO.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short InternalSourceFault : 1;
				/// Поле Поле XT1LFOFFG. Флаг ошибки XT1 в режиме низкой частоты.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short ExternalSource1LowFreqFault : 1;
				/// Поле XT1HFOFFG. Флаг ошибки XT1 в режиме высокой частоты.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short ExternalSource1HighFreqFault : 1;
				/// Поле XT2OFFG. Флаг ошибки XT2.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short ExternalSource2Fault : 1;
				unsigned short Reserved0 : 12;
			};
			struct
			{
				/// Объединённое поле. Все флаги ошибок.
				/// \memberof ClockSourcFaultFlagsRegister
				unsigned short SourceFaults : 4; // DOFFG, XT1LFOFFG, XT1HFOFFG, XT2OFFG
				unsigned short Reserved00 : 12;
			};
		};

	}
}
}

#endif // CLOCK_SYSTEM_REGISTERS_H
