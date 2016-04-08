#ifndef WDT_EMF32_H
#define WDT_EMF32_H

#include "Register.h"
#include "../IdObjectBase.h"

namespace Rblib
{
namespace Efm32
{
namespace Wdt
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// WDOG_CTRL - Control Register (Async Reg)
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum ClockSource
		{
			ClockSourceULFRCO		= 0,
			ClockSourceLFRCO		= 1,
			ClockSourceLFXO			= 2
		};
		
		enum TimeoutPeriod
		{
			TimeoutPeriod9		= 0,
			TimeoutPeriod17		= 1,
			TimeoutPeriod33		= 2,
			TimeoutPeriod65		= 3,
			TimeoutPeriod129	= 4,
			TimeoutPeriod257	= 5,
			TimeoutPeriod513	= 6,
			TimeoutPeriod1k		= 7,
			TimeoutPeriod2k		= 8,
			TimeoutPeriod4k		= 9,
			TimeoutPeriod8k		= 10,
			TimeoutPeriod16k	= 11,
			TimeoutPeriod32k	= 12,
			TimeoutPeriod64k	= 13,
			TimeoutPeriod128k	= 14,
			TimeoutPeriod256k	= 15,
		};
	protected:
		struct RegStruct
		{
			ValueDataType WatchdogTimerEnable							: 1;
			ValueDataType DebugModeRunEnable							: 1;
			ValueDataType EnergyMode2RunEnable							: 1;
			ValueDataType EnergyMode3RunEnable							: 1;
			ValueDataType ConfigurationLock								: 1;
			ValueDataType EnergyMode4Block								: 1;
			ValueDataType SoftwareOscillatorDisableBlock				: 1;
			ValueDataType 												: 1;
			ValueDataType WatchdogTimeoutPeriodSelect					: 4;
			ValueDataType WatchdogClockSelect							: 2;
			ValueDataType 												: 18;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Enable(ClockSource clockSource, TimeoutPeriod timeoutPeriod, bool energyMode2RunEnable = true, bool energyMode3RunEnable = true)
		{
			Register::Ptr()->WatchdogTimerEnable = 1;
			Register::Ptr()->WatchdogClockSelect = clockSource;
			Register::Ptr()->WatchdogTimeoutPeriodSelect = timeoutPeriod;
			Register::Ptr()->EnergyMode2RunEnable = energyMode2RunEnable;
			Register::Ptr()->EnergyMode3RunEnable = energyMode3RunEnable;
		}
		
		static void Disable()
		{
			Register::Ptr()->WatchdogTimerEnable = 0;
		}
	};
	
	// WDOG_CMD - Command Register (Async Reg)
	template
		<
			RegisterAddressType Address
		>
	class CommandRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Clear		: 1;
			ValueDataType 			: 31;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Clear()
		{
			Register::Ptr()->Clear = 1;
		}
	};
	
	// WDOG_SYNCBUSY - Synchronization Busy Register
	template
		<
			RegisterAddressType Address
		>
	class SynchronizationBusyRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType WDOG_CMDRegisterBusy		: 1;
			ValueDataType WDOG_CTRLRegisterBusy		: 1;
			ValueDataType 							: 30;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
	};
	
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType SynchronizationBusyRegAddr
		>
	class Wdt :
		public IdObjectBase<IdObj>,
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public CommandRegister<BaseAddr + CommandRegAddr>,
		public SynchronizationBusyRegister<BaseAddr + SynchronizationBusyRegAddr>
	{
	};
}
}
}

#endif