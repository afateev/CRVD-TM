#ifndef EMF32_LETIMER_H
#define EMF32_LETIMER_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace LeTimer
{
	// LETIMERn_CTRL - Control Register (Async Reg)
	template
		<
			Core::RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum Mode
		{
			ModeFree				= 0,
			ModeOneShot				= 1,
			ModeBuffered			= 2,
			ModeDouble				= 3
		};
		
		enum Divider
		{
			Divider1				= 0,
			Divider2				= 1,
			Divider4				= 2,
			Divider8				= 3,
			Divider16				= 4,
			Divider32				= 5,
			Divider64				= 6,
			Divider128				= 7,
			Divider256				= 8,
			Divider512				= 9,
			Divider1024				= 10
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType RepeatMode									: 2;
			Core::RegisterValueType UnderflowOutputAction0						: 2;
			Core::RegisterValueType UnderflowOutputAction1						: 2;
			Core::RegisterValueType Output0Polarity								: 1;
			Core::RegisterValueType Output1Polarity								: 1;
			Core::RegisterValueType BufferedTop									: 1;
			Core::RegisterValueType CompareValue0IsTopValue						: 1;
			Core::RegisterValueType RtcCompare0TriggerEnable					: 1;
			Core::RegisterValueType RtcCompare1TriggerEnable					: 1;
			Core::RegisterValueType DebugModeRunEnable							: 1;
			Core::RegisterValueType 											: 19;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetRepeateMode(Mode mode)
		{
			Register::Ptr()->RepeatMode = mode;
		}
		
		static void SetCompareValue0IsTopValue(bool value)
		{
			Register::Ptr()->CompareValue0IsTopValue = value;
		}
	};
	
	// LETIMERn_CMD - Command Register
	template
		<
			Core::RegisterAddressType Address
		>
	class CommandRegister
	{
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Start						: 1;
			Core::RegisterValueType Stop						: 1;
			Core::RegisterValueType Clear						: 1;
			Core::RegisterValueType ClearToggleOutput0			: 1;
			Core::RegisterValueType ClearToggleOutput1			: 1;
			Core::RegisterValueType 							: 27;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Start()
		{
			Register::Ptr()->Start = 1;
		}
		
		static void Stop()
		{
			Register::Ptr()->Stop = 1;
		}
		
		static void Clear()
		{
			Register::Ptr()->Stop = 1;
		}
	};
	
	// LETIMERn_STATUS - Status Register
	template
		<
			Core::RegisterAddressType Address
		>
	class StatusRegister
	{
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Running					: 1;
			Core::RegisterValueType 						: 31;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool Running()
		{
			return Register::Ptr()->Running;
		}
	};
	
	// LETIMERn_CNT - Counter Value Register
	template
		<
			Core::RegisterAddressType Address
		>
	class CounterValueRegister
	{
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CounterValue		: 16;
			Core::RegisterValueType 					: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType GetCounter()
		{
			return Register::Ptr()->CounterValue;
		}
	};

	// LETIMERn_COMP0 - Compare Value Register 0 (Async Reg)
	template
		<
			Core::RegisterAddressType Address
		>
	class Compare0ValueRegister
	{
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Value		: 16;
			Core::RegisterValueType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Compare0SetValue(Core::RegisterValueType val)
		{
			Register::Ptr()->Value = val;
		}
	};

	// LETIMERn_COMP1 - Compare Value Register 1 (Async Reg)
	template
		<
			Core::RegisterAddressType Address
		>
	class Compare1ValueRegister
	{
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Value		: 16;
			Core::RegisterValueType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Compare1SetValue(Core::RegisterValueType val)
		{
			Register::Ptr()->Value = val;
		}
	};

	// LETIMERn_REP0 - Repeat Counter Register 0 (Async Reg)
	template
		<
			Core::RegisterAddressType Address
		>
	class RepeatCounter0Register
	{
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Value		: 8;
			Core::RegisterValueType 			: 24;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType RepeatCounter0GetValue(Core::RegisterValueType val)
		{
			return Register::Ptr()->Value;
		}
		
		static void RepeatCounter0SetValue(Core::RegisterValueType val)
		{
			Register::Ptr()->Value = val;
		}
	};

	// LETIMERn_REP1 - Repeat Counter Register 1 (Async Reg)
	template
		<
			Core::RegisterAddressType Address
		>
	class RepeatCounter1Register
	{
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Value		: 8;
			Core::RegisterValueType 			: 24;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType RepeatCounter0GetValue(Core::RegisterValueType val)
		{
			return Register::Ptr()->Value;
		}
		
		static void RepeatCounter1SetValue(Core::RegisterValueType val)
		{
			Register::Ptr()->Value = val;
		}
	};

	// LETIMERn_IF - Interrupt Flag Register
	template
		<
			Core::RegisterAddressType Address
		>
	class InterruptFlagRegister
	{
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagCompareMatch0							= 0,
			InterruptFlagCompareMatch1							= 1,
			InterruptFlagUnderflow								= 2,
			InterruptFlagRepeatCounter0							= 3,
			InterruptFlagRepeatCounter1							= 4,
		};
	public:
		static bool GetInterruptFlag(InterruptFlag i)
		{
			return *Register::Ptr() & (1 << (unsigned char)i);
		}
	};

	// LETIMERn_IFS - Interrupt Flag Set Register
	template
		<
			Core::RegisterAddressType Address
		>
	class InterruptFlagSetRegister
	{
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagCompareMatch0							= 0,
			InterruptFlagCompareMatch1							= 1,
			InterruptFlagUnderflow								= 2,
			InterruptFlagRepeatCounter0							= 3,
			InterruptFlagRepeatCounter1							= 4,
		};
	public:
		static void SetInterruptFlag(InterruptFlag i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};

	// LETIMERn_IFC - Interrupt Flag Clear Register
	template
		<
			Core::RegisterAddressType Address
		>
	class InterruptFlagClearRegister
	{
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagCompareMatch0							= 0,
			InterruptFlagCompareMatch1							= 1,
			InterruptFlagUnderflow								= 2,
			InterruptFlagRepeatCounter0							= 3,
			InterruptFlagRepeatCounter1							= 4,
		};
	public:
		static void ClearInterruptFlag(InterruptFlag i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};

	// LETIMERn_IEN - Interrupt Enable Register
	template
		<
			Core::RegisterAddressType Address
		>
	class InterruptEnableRegister
	{
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum Interrupt
		{
			InterruptCompareMatch0							= 0,
			InterruptCompareMatch1							= 1,
			InterruptUnderflow								= 2,
			InterruptRepeatCounter0							= 3,
			InterruptRepeatCounter1							= 4,
		};
	public:
		static void InterruptEnable(Interrupt i)
		{
			*Register::Ptr() |= (1 << (unsigned char)i);
		}
		
		static void InterruptDisable(Interrupt i)
		{
			*Register::Ptr() &= ~(1 << (unsigned char)i);
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
		static HandlerType CompareMatch0InterruptHandler;
		static HandlerType CompareMatch1InterruptHandler;
		static HandlerType UnderflowInterruptHandler;
	};
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::CompareMatch0InterruptHandler;
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::CompareMatch1InterruptHandler;
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::UnderflowInterruptHandler;
	
	template
		<
			unsigned int IdObj,
			Core::RegisterAddressType BaseAddr,
			Core::RegisterAddressType ControlRegAddr,
			Core::RegisterAddressType CommandRegAddr,
			Core::RegisterAddressType StatusRegAddr,
			Core::RegisterAddressType CounterValueRegisterAddr,
			Core::RegisterAddressType CompareValueRegister0Addr,
			Core::RegisterAddressType CompareValueRegister1Addr,
			Core::RegisterAddressType RepeatCounterRegister0Addr,
			Core::RegisterAddressType RepeatCounterRegister1Addr,
			Core::RegisterAddressType InterruptFlagRegAddr,
			Core::RegisterAddressType InterruptFlagSetRegAddr,
			Core::RegisterAddressType InterruptFlagClearRegAddr,
			Core::RegisterAddressType InterruptEnableRegAddr,
			Core::RegisterAddressType FreezeRegister,
			Core::RegisterAddressType SynchronizationBusyRegister,
			Core::RegisterAddressType IORoutingRegAddr
		>
	class LeTimer :
		public IdObjectBase<IdObj>,
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public CommandRegister<BaseAddr + CommandRegAddr>,
		public StatusRegister<BaseAddr + StatusRegAddr>,
		public CounterValueRegister<BaseAddr + CounterValueRegisterAddr>,
		public Compare0ValueRegister<BaseAddr + CompareValueRegister0Addr>,
		public Compare1ValueRegister<BaseAddr + CompareValueRegister1Addr>,
		public RepeatCounter0Register<BaseAddr + RepeatCounterRegister0Addr>,
		public RepeatCounter1Register<BaseAddr + RepeatCounterRegister1Addr>,
		public InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr>,
		public InterruptFlagSetRegister<BaseAddr + InterruptFlagSetRegAddr>,
		public InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr>,
		public InterruptEnableRegister<BaseAddr + InterruptEnableRegAddr>,
		public StaticParams<IdObj>
	{
	public:
		typedef InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr> InterruptFlagRegister;
		typedef InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr> InterruptFlagClearRegister;
		typedef StaticParams<IdObj> StaticParams;
	public:
		static void InterruptHandler()
		{
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagCompareMatch0))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagCompareMatch0);
				StaticParams::CompareMatch0InterruptHandler.Call();
			}
			
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagCompareMatch1))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagCompareMatch1);
				StaticParams::CompareMatch1InterruptHandler.Call();
			}
			
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagUnderflow))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagUnderflow);
				StaticParams::UnderflowInterruptHandler.Call();
			}
		}

	};
}
}
}

#endif
