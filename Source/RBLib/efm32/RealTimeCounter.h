#ifndef EMF32_REALTIMECOUNTER_H
#define EMF32_REALTIMECOUNTER_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace RealTimeCounter
{
	static const unsigned int BaseAddress = 0x40080000;
	
	//RTC_CTRL - Control Register
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ControlRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x000;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Enable									: 1;
			Core::RegisterValueType DebugModeRunEnable						: 1;
			Core::RegisterValueType CompareChannel0IsTopValue				: 1;
			Core::RegisterValueType											: 29;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Enable(bool enable = true)
		{
			Register::Ptr()->Enable = enable;
		}
		
		static void Start()
		{
			Enable();
		}
		
		static void Stop()
		{
			Enable(false);
		}
		
		static void Clear()
		{
			Enable(false);
		}
		
		static void SetCompareValue0IsTopValue(bool value)
		{
			Register::Ptr()->CompareChannel0IsTopValue = value;
		}
	};
	
	// RTC_CNT - Counter Value Register
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class CounterValueRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x004;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType CounterValue		: 24;
			Core::RegisterValueType 					: 8;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static Core::RegisterValueType GetCounter()
		{
			return Register::Ptr()->CounterValue;
		}
	};
	
	// RTC_COMP0 - Compare Value Register 0
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class Compare0ValueRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x008;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Value		: 24;
			Core::RegisterValueType 			: 8;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Compare0SetValue(Core::RegisterValueType val)
		{
			Register::Ptr()->Value = val;
		}
	};
	
	// RTC_COMP1 - Compare Value Register 1
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class Compare1ValueRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType Value		: 24;
			Core::RegisterValueType 			: 8;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Compare1SetValue(Core::RegisterValueType val)
		{
			Register::Ptr()->Value = val;
		}
	};
	
	// RTC_IF - Interrupt Flag Register
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class InterruptFlagRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x010;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagOverflow								= 0,
			InterruptFlagCompareMatch0							= 1,
			InterruptFlagCompareMatch1							= 2,
		};
	public:
		static bool GetInterruptFlag(InterruptFlag i)
		{
			return *Register::Ptr() & (1 << (unsigned char)i);
		}
	};
	
	//RTC_IFS - Interrupt Flag Set Register
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class InterruptFlagSetRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x014;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagOverflow								= 0,
			InterruptFlagCompareMatch0							= 1,
			InterruptFlagCompareMatch1							= 2,
		};
	public:
		static void SetInterruptFlag(InterruptFlag i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};
	
	// RTC_IFC - Interrupt Flag Clear Register
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class InterruptFlagClearRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x018;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagOverflow								= 0,
			InterruptFlagCompareMatch0							= 1,
			InterruptFlagCompareMatch1							= 2,
		};
	public:
		static void ClearInterruptFlag(InterruptFlag i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};
	
	// RTC_IEN - Interrupt Enable Register
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class InterruptEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x01C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		enum Interrupt
		{
			InterruptFlagOverflow								= 0,
			InterruptFlagCompareMatch0							= 1,
			InterruptFlagCompareMatch1							= 2,
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
		static HandlerType OverflowInterruptHandler;
		static HandlerType CompareMatch0InterruptHandler;
		static HandlerType CompareMatch1InterruptHandler;
	};
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::OverflowInterruptHandler;
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::CompareMatch0InterruptHandler;
	
	template <int IdObj>
		StaticParams<IdObj>::HandlerType StaticParams<IdObj>::CompareMatch1InterruptHandler;
	
	template
		<
			unsigned int IdObj
		>
	class RealTimeCounter :
		public IdObjectBase<IdObj>,
		public ControlRegister<BaseAddress>,
		public CounterValueRegister<BaseAddress>,
		public Compare0ValueRegister<BaseAddress>,
		public Compare1ValueRegister<BaseAddress>,
		public InterruptFlagRegister<BaseAddress>,
		public InterruptFlagSetRegister<BaseAddress>,
		public InterruptFlagClearRegister<BaseAddress>,
		public InterruptEnableRegister<BaseAddress>,
		public StaticParams<IdObj>
	{
	public:
		typedef InterruptFlagRegister<BaseAddress> InterruptFlagRegister;
		typedef InterruptFlagClearRegister<BaseAddress> InterruptFlagClearRegister;
		typedef StaticParams<IdObj> StaticParams;
	public:
		static void InterruptHandler()
		{
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagOverflow))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagOverflow);
				StaticParams::OverflowInterruptHandler.Call();
			}
			
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
		}
	};
}
}
}

#endif