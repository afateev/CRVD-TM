#ifndef TIMER_EMF32_H
#define TIMER_EMF32_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace Timer
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// TIMERn_CTRL - Control Register
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum Mode
		{
			ModeUp					= 0,
			ModeDown				= 1,
			ModeUpDown				= 2,
			ModeQuadratureDecoder	= 3
		};
		enum ClockSource
		{
			ClockSourcePrescaledHFPERCLK				= 0,
			ClockSourceCCChannel1Input					= 1,
			ClockSourceNeighborTimerOverflowUnderflow	= 2
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
			ValueDataType TimerMode										: 2;
			ValueDataType 												: 1;
			ValueDataType TimerStartStopReloadSynchronization			: 1;
			ValueDataType OneShotModeEnable								: 1;
			ValueDataType QuadratureDecoderModeSelection				: 1;
			ValueDataType DebugModeRunEnable							: 1;
			ValueDataType DMARequestClearOnActive						: 1;
			ValueDataType TimerRisingInputEdgeAction					: 2;
			ValueDataType TimerFallingInputEdgeAction					: 2;
			ValueDataType 												: 1;
			ValueDataType CountMode2x									: 1;
			ValueDataType 												: 2;
			ValueDataType ClockSourceSelect								: 2;
			ValueDataType 												: 6;
			ValueDataType PrescalerSetting								: 4;
			ValueDataType AlwaysTrackInputs								: 1;
			ValueDataType ReloadStartSetsCompareOuptutInitialState		: 1;
			ValueDataType 												: 2;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetMode(Mode mode, bool oneShot = false)
		{
			Register::Ptr()->TimerMode = mode;
			Register::Ptr()->OneShotModeEnable = oneShot;
		}
		
		static void SetClockSource(ClockSource val)
		{
			Register::Ptr()->ClockSourceSelect = val;
		}
		
		static void SetDivider(Divider val)
		{
			Register::Ptr()->PrescalerSetting = val;
		}
	};
	
	// TIMERn_CMD - Command Register
	template
		<
			RegisterAddressType Address
		>
	class CommandRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Start		: 1;
			ValueDataType Stop		: 1;
			ValueDataType 			: 30;
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
	};
	
	// TIMERn_IEN - Interrupt Enable Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptEnableRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum Interrupt
		{
			InterruptOverflow								= 0,
			InterruptUnderflow								= 1,
			InterruptCCChannel0								= 4,
			InterruptCCChannel1								= 5,
			InterruptCCChannel2					 			= 6,
			InterruptCCChannel0InputCaptureBufferOverflow	= 8,
			InterruptCCChannel1InputCaptureBufferOverflow	= 9,
			InterruptCCChannel2InputCaptureBufferOverflow	= 10
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
	
	// TIMERn_IF - Interrupt Flag Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagOverflow								= 0,
			InterruptFlagUnderflow								= 1,
			InterruptFlagCCChannel0								= 4,
			InterruptFlagCCChannel1								= 5,
			InterruptFlagCCChannel2					 			= 6,
			InterruptFlagCCChannel0InputCaptureBufferOverflow	= 8,
			InterruptFlagCCChannel1InputCaptureBufferOverflow	= 9,
			InterruptFlagCCChannel2InputCaptureBufferOverflow	= 10
		};
	public:
		static bool GetInterruptFlag(InterruptFlag i)
		{
			return *Register::Ptr() & (1 << (unsigned char)i);
		}
	};
	
	// TIMERn_IFS - Interrupt Flag Set Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagSetRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagOverflow								= 0,
			InterruptFlagUnderflow								= 1,
			InterruptFlagCCChannel0								= 4,
			InterruptFlagCCChannel1								= 5,
			InterruptFlagCCChannel2					 			= 6,
			InterruptFlagCCChannel0InputCaptureBufferOverflow	= 8,
			InterruptFlagCCChannel1InputCaptureBufferOverflow	= 9,
			InterruptFlagCCChannel2InputCaptureBufferOverflow	= 10
		};
	public:
		static void SetInterruptFlag(InterruptFlag i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};
	
	// TIMERn_IFC - Interrupt Flag Clear Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagClearRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		enum InterruptFlag
		{
			InterruptFlagOverflow								= 0,
			InterruptFlagUnderflow								= 1,
			InterruptFlagCCChannel0								= 4,
			InterruptFlagCCChannel1								= 5,
			InterruptFlagCCChannel2					 			= 6,
			InterruptFlagCCChannel0InputCaptureBufferOverflow	= 8,
			InterruptFlagCCChannel1InputCaptureBufferOverflow	= 9,
			InterruptFlagCCChannel2InputCaptureBufferOverflow	= 10
		};
	public:
		static void ClearInterruptFlag(InterruptFlag i)
		{
			*Register::Ptr() = (1 << (unsigned char)i);
		}
	};
	
	// TIMERn_TOP - Counter Top Value Register
	template
		<
			RegisterAddressType Address
		>
	class CounterTopValueRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType CounterTopValue		: 16;
			ValueDataType 						: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetMaxCount(ValueDataType val)
		{
			Register::Ptr()->CounterTopValue = val;
		}
	};
	
	// TIMERn_CNT - Counter Value Register
	template
		<
			RegisterAddressType Address
		>
	class CounterValueRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType CounterValue		: 16;
			ValueDataType 						: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static ValueDataType GetCounter()
		{
			return Register::Ptr()->CounterValue;
		}
		
		static void SetCounter(ValueDataType val)
		{
			Register::Ptr()->CounterValue = val;
		}
	};
	
	// TIMERn_CC0_CTRL - CC Channel Control Register
	template
		<
			RegisterAddressType Address
		>
	class CCChannel0ControlRegister
	{
	public:
		enum Mode
		{
			ModeOff				= 0,
			ModeInputCapture	= 1,
			ModeOutputCompare	= 2,
			ModePWM				= 3
		};
	protected:
		struct RegStruct
		{
			ValueDataType CCChannelMode						: 2;
			ValueDataType OutputInvert						: 1;
			ValueDataType 									: 1;
			ValueDataType CompareOutputInitialState			: 1;
			ValueDataType 									: 3;
			ValueDataType CompareMatchOutputAction			: 2;
			ValueDataType CounterOverflowOutputAction		: 2;
			ValueDataType CounterUnderflowOutputAction		: 2;
			ValueDataType 									: 2;
			ValueDataType PRSInputChannelSelection			: 4;
			ValueDataType InputSelection					: 1;
			ValueDataType DigitalFilter						: 1;
			ValueDataType 									: 2;
			ValueDataType InputCaptureEdgeSelect			: 2;
			ValueDataType InputCaptureEventControl			: 2;
			ValueDataType 									: 4;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void CaptureCompare0Disable()
		{
			Register::Ptr()->CCChannelMode = ModeOff;
		}
		
		static void Capture0Enable()
		{
			Register::Ptr()->CCChannelMode = ModeOutputCompare;
		}
		
		static void Compare0Enable()
		{
			Register::Ptr()->CCChannelMode = ModeOutputCompare;
		}
	};
	
	// TIMERn_CC0_CCV - CC Channel Value Register
	template
		<
			RegisterAddressType Address
		>
	class CCChannel0ValueRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Value		: 16;
			ValueDataType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static ValueDataType Capture0GetValue()
		{
			return Register::Ptr()->Value;
		}
		
		static void Compare0SetValue(ValueDataType val)
		{
			Register::Ptr()->Value = val;
		}
	};
	
	// TIMERn_CC1_CTRL - CC Channel Control Register
	template
		<
			RegisterAddressType Address
		>
	class CCChannel1ControlRegister
	{
	public:
		enum Mode
		{
			ModeOff				= 0,
			ModeInputCapture	= 1,
			ModeOutputCompare	= 2,
			ModePWM				= 3
		};
	protected:
		struct RegStruct
		{
			ValueDataType CCChannelMode						: 2;
			ValueDataType OutputInvert						: 1;
			ValueDataType 									: 1;
			ValueDataType CompareOutputInitialState			: 1;
			ValueDataType 									: 3;
			ValueDataType CompareMatchOutputAction			: 2;
			ValueDataType CounterOverflowOutputAction		: 2;
			ValueDataType CounterUnderflowOutputAction		: 2;
			ValueDataType 									: 2;
			ValueDataType PRSInputChannelSelection			: 4;
			ValueDataType InputSelection					: 1;
			ValueDataType DigitalFilter						: 1;
			ValueDataType 									: 2;
			ValueDataType InputCaptureEdgeSelect			: 2;
			ValueDataType InputCaptureEventControl			: 2;
			ValueDataType 									: 4;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void CaptureCompare1Disable()
		{
			Register::Ptr()->CCChannelMode = ModeOff;
		}
		
		static void Capture1Enable()
		{
			Register::Ptr()->CCChannelMode = ModeOutputCompare;
		}
		
		static void Compare1Enable()
		{
			Register::Ptr()->CCChannelMode = ModeOutputCompare;
		}
	};
	
	// TIMERn_CC1_CCV - CC Channel Value Register
	template
		<
			RegisterAddressType Address
		>
	class CCChannel1ValueRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Value		: 16;
			ValueDataType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static ValueDataType Capture1GetValue()
		{
			return Register::Ptr()->Value;
		}
		
		static void Compare1SetValue(ValueDataType val)
		{
			Register::Ptr()->Value = val;
		}
	};
	
	// TIMERn_CC2_CTRL - CC Channel Control Register
	template
		<
			RegisterAddressType Address
		>
	class CCChannel2ControlRegister
	{
	public:
		enum Mode
		{
			ModeOff				= 0,
			ModeInputCapture	= 1,
			ModeOutputCompare	= 2,
			ModePWM				= 3
		};
	protected:
		struct RegStruct
		{
			ValueDataType CCChannelMode						: 2;
			ValueDataType OutputInvert						: 1;
			ValueDataType 									: 1;
			ValueDataType CompareOutputInitialState			: 1;
			ValueDataType 									: 3;
			ValueDataType CompareMatchOutputAction			: 2;
			ValueDataType CounterOverflowOutputAction		: 2;
			ValueDataType CounterUnderflowOutputAction		: 2;
			ValueDataType 									: 2;
			ValueDataType PRSInputChannelSelection			: 4;
			ValueDataType InputSelection					: 1;
			ValueDataType DigitalFilter						: 1;
			ValueDataType 									: 2;
			ValueDataType InputCaptureEdgeSelect			: 2;
			ValueDataType InputCaptureEventControl			: 2;
			ValueDataType 									: 4;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void CaptureCompare2Disable()
		{
			Register::Ptr()->CCChannelMode = ModeOff;
		}
		
		static void Capture2Enable()
		{
			Register::Ptr()->CCChannelMode = ModeOutputCompare;
		}
		
		static void Compare2Enable()
		{
			Register::Ptr()->CCChannelMode = ModeOutputCompare;
		}
	};
	
	// TIMERn_CC2_CCV - CC Channel Value Register
	template
		<
			RegisterAddressType Address
		>
	class CCChannel2ValueRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Value		: 16;
			ValueDataType 			: 16;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static ValueDataType Capture2GetValue()
		{
			return Register::Ptr()->Value;
		}
		
		static void Compare2SetValue(ValueDataType val)
		{
			Register::Ptr()->Value = val;
		}
	};
	
	template
		<
			int IdObj
		>
	class StaticParams
	{
	public:
		typedef CallbackWrapper<int> CompareMatchHandlerType;
		typedef CallbackWrapper<> OverflowHandlerType;
	public:
		static CompareMatchHandlerType CompareMatchInterruptHandler;
		static OverflowHandlerType OverflowInterruptHandler;
	};
	
	template <int IdObj>
		StaticParams<IdObj>::CompareMatchHandlerType StaticParams<IdObj>::CompareMatchInterruptHandler;
	
	template <int IdObj>
		StaticParams<IdObj>::OverflowHandlerType StaticParams<IdObj>::OverflowInterruptHandler;
	
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType StatusRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType CounterTopValueRegAddr,
			RegisterAddressType CounterTopValueBufferRegAddr,
			RegisterAddressType CounterValueRegAddr,
			RegisterAddressType IORoutingRegAddr,
			RegisterAddressType CCChannel0ControlRegAddr,
			RegisterAddressType CCChannel0ValueRegAddr,
			RegisterAddressType CCChannel0ValuePeekRegAddr,
			RegisterAddressType CCChanne0lBufferRegAddr,
			RegisterAddressType CCChannel1ControlRegAddr,
			RegisterAddressType CCChannel1ValueRegAddr,
			RegisterAddressType CCChannel1ValuePeekRegAddr,
			RegisterAddressType CCChannel1BufferRegAddr,
			RegisterAddressType CCChannel2ControlRegAddr,
			RegisterAddressType CCChannel2ValueRegAddr,
			RegisterAddressType CCChannel2ValuePeekRegAddr,
			RegisterAddressType CCChannel2BufferRegAddr,
			RegisterAddressType DTIControlRegAddr,
			RegisterAddressType DTITimeControlRegAddr,
			RegisterAddressType DTIFaultConfigurationRegAddr,
			RegisterAddressType DTIOutputGenerationEnableRegAddr,
			RegisterAddressType DTIFaultRegAddr,
			RegisterAddressType DTIFaultClearRegAddr,
			RegisterAddressType DTIConfigurationLockRegAddr
		>
	class Timer :
		public IdObjectBase<IdObj>,
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public CommandRegister<BaseAddr + CommandRegAddr>,
		public InterruptEnableRegister<BaseAddr + InterruptEnableRegAddr>,
		public InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr>,
		public InterruptFlagSetRegister<BaseAddr + InterruptFlagSetRegAddr>,
		public InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr>,
		public CounterTopValueRegister<BaseAddr + CounterTopValueRegAddr>,
		public CounterValueRegister<BaseAddr + CounterValueRegAddr>,
		public CCChannel0ControlRegister<BaseAddr + CCChannel0ControlRegAddr>,
		public CCChannel0ValueRegister<BaseAddr + CCChannel0ValueRegAddr>,
		public CCChannel1ControlRegister<BaseAddr + CCChannel1ControlRegAddr>,
		public CCChannel1ValueRegister<BaseAddr + CCChannel1ValueRegAddr>,
		public CCChannel2ControlRegister<BaseAddr + CCChannel2ControlRegAddr>,
		public CCChannel2ValueRegister<BaseAddr + CCChannel2ValueRegAddr>,
		public StaticParams<IdObj>
	{
	public:
		typedef InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr> InterruptFlagRegister;
		typedef InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr> InterruptFlagClearRegister;
		typedef StaticParams<IdObj> StaticParams;
	public:
		static void InterruptHandler()
		{
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagCCChannel0))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagCCChannel0);
				
				StaticParams::CompareMatchInterruptHandler.Call(0);
			}
			
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagCCChannel1))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagCCChannel1);
				
				StaticParams::CompareMatchInterruptHandler.Call(1);
			}
			
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagCCChannel2))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagCCChannel2);
				
				StaticParams::CompareMatchInterruptHandler.Call(2);
			}
			
			if (InterruptFlagRegister::GetInterruptFlag(InterruptFlagRegister::InterruptFlagOverflow))
			{
				InterruptFlagClearRegister::ClearInterruptFlag(InterruptFlagClearRegister::InterruptFlagOverflow);
				
				StaticParams::OverflowInterruptHandler.Call();
			}
		}
	};
}
}
}

#endif
