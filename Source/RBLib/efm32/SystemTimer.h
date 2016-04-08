#ifndef EMF32_SYSTEMTIMER_H
#define EMF32_SYSTEMTIMER_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace SystemTimer
{
	// SysTick CTRL - SysTick Control and Status Register
	class ControlAndStatusRegister
	{
	public:
		static const unsigned int Address = 0xE000E010;
		
		enum ClockSource
		{
			ClockSourceRtc = 0,
			ClockSourceHFCORECLK = 1,
		};	
	protected:
		struct RegStruct
		{
			Core::RegisterValueType ENABLE		: 1; // Enables the counter
			Core::RegisterValueType TICKINT		: 1; // Enables SysTick exception request
			Core::RegisterValueType CLKSOURCE	: 1; // Indicates the clock source
			Core::RegisterValueType 			: 13;
			Core::RegisterValueType COUNTFLAG	: 1; // Returns 1 if timer counted to 0 since last time this was read
			Core::RegisterValueType 			: 15;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Enable(bool enable = true)
		{
			Register::Ptr()->ENABLE = enable;
		}
		
		static void EnableInterrupt(bool enable = true)
		{
			Register::Ptr()->TICKINT = enable;
		}
		
		static void SelectClockSource(ClockSource clockSource)
		{
			Register::Ptr()->CLKSOURCE = clockSource;
		}
	};
	
	// SysTick LOAD - SysTick Reload Value Register
	class ReloadValueRegister
	{
	public:
		static const unsigned int Address = 0xE000E014;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType RELOAD			: 24; // Value to load into the VAL register when the counter is enabled and when it reaches 0
			Core::RegisterValueType 				: 7;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetMaxCount(unsigned int value)
		{
			Register::Ptr()->RELOAD = value & 0x00FFFFFF;
		}
	};
	
	// SysTick VAL - SysTick Current Value Register
	class CurrentValueRegister
	{
	public:
		static const unsigned int Address = 0xE000E018;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CURRENT			: 24; // Reads return the current value of the SysTick counter
			Core::RegisterValueType 				: 7;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Reset()
		{
			Register::Ptr()->CURRENT = 0;
		}
	};
	
	template
		<
			int IdObj
		>
	class StaticParams
	{
	public:
		typedef CallbackWrapper<> HandlerType;
	public:
		static HandlerType SysTickInterruptHandler;
	};
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::SysTickInterruptHandler;
	
	class SystemTimer :
		public ControlAndStatusRegister,
		public ReloadValueRegister,
		public CurrentValueRegister,
		public StaticParams<0>
	{
	public:
		static void InterruptHandler()
		{
			StaticParams::SysTickInterruptHandler();
		}
	};
}
}
}

#endif