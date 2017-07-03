///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ таймерами
//	ATMEGA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef TIMER_ATMEGA_H
#define TIMER_ATMEGA_H

#include "../IdObjectBase.h"
#include "../RegisterReference.h"

namespace Lib
{
namespace ATmega
{
namespace Timer
{
	// “ип значени€ порта
	typedef unsigned char ValueDataType;
	
	// TCCRxA Ц Timer/Counter Control Register A
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlRegisterA
	{
	public:
		struct RegisterStruct
		{
			ValueDataType WaveformGenerationMode01 	: 2;
			ValueDataType 			 				: 2;
			ValueDataType CompareMatchOutputBMode 	: 2;
		};
		
		enum WaveformGenerationMode
		{
			WaveformGenerationModeNormal					= 0,
			WaveformGenerationModePwm						= 1,
			WaveformGenerationModeClearTimerOnCompareMatch	= 2,
			WaveformGenerationModeFastPwm					= 3
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetWaveformGenerationMode(WaveformGenerationMode mode)
		{
			RegRef::_register->WaveformGenerationMode01 = mode;
		}
	};
	
	// TCCRxB Ц Timer/Counter Control Register B
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlRegisterB
	{
	public:
		struct RegisterStruct
		{
			ValueDataType ClockSelect 				: 3;
			ValueDataType WaveformGenerationMode2 	: 1;
			ValueDataType 			 				: 2;
			ValueDataType ForceOutputCompareB 		: 1;
			ValueDataType ForceOutputCompareA 		: 1;
		};
		
		enum ClockSource
		{
			ClockSourceStopped					= 0,
			ClockSourceFromPrescalerDiv1		= 1,
			ClockSourceFromPrescalerDiv8		= 2,
			ClockSourceFromPrescalerDiv64		= 3,
			ClockSourceFromPrescalerDiv256		= 4,
			ClockSourceFromPrescalerDiv1204		= 5,
			ClockSourceFromPinOnFallingEdge		= 6,
			ClockSourceFromPinOnRisingEdge		= 7
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void Start(ClockSource clockSource)
		{
			RegRef::_register->ClockSelect = clockSource;
		}
		
		static void Stop()
		{
			RegRef::_register->ClockSelect = ClockSourceStopped;
		}
	};
	
	// TCNTx Ц Timer/Counter Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class CounterRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void SetCounter(unsigned char val)
		{
			*RegRef::_register = val;
		}
		
		static unsigned char GetCounter()
		{
			return *RegRef::_register;
		}
		
		static void Reset()
		{
			SetCounter(0);
		}
	};
	
	// TOCRxA Ц Output Compare Register A
	template
		<
			RegisterPointerType RegisterAddr
		>
	class OutputCompareARegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void SetOutputCompareAValue(unsigned char val)
		{
			*RegRef::_register = val;
		}
	};
	
	// TOCRxA Ц Output Compare Register B
	template
		<
			RegisterPointerType RegisterAddr
		>
	class OutputCompareBRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void SetOutputCompareBValue(unsigned char val)
		{
			*RegRef::_register = val;
		}
	};
	
	// TIMSKx Ц Timer/Counter Interrupt Mask Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class InterruptMaskRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType OverflowInterruptEnable 				: 1;
			ValueDataType OutputCompareMatchAInterruptEnable 	: 1;
			ValueDataType OutputCompareMatchBInterruptEnable 	: 1;
			ValueDataType 			 							: 5;
		};
		
		enum Interrupt
		{
			InterruptOverflow,
			InterruptOutputCompareMatchA,
			InterruptOutputCompareMatchB
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetInterruptEnable(Interrupt interrupt, bool enable = true)
		{
			switch(interrupt)
			{
			case InterruptOverflow:
				RegRef::_register->OverflowInterruptEnable = enable;
				break;
			case InterruptOutputCompareMatchA:
				RegRef::_register->OutputCompareMatchAInterruptEnable = enable;
				break;
			case InterruptOutputCompareMatchB:
				RegRef::_register->OutputCompareMatchBInterruptEnable = enable;
				break;
			}
		}
		
		static void InterruptEnable(Interrupt interrupt)
		{
			SetInterruptEnable(interrupt, true);
		}
		
		static void InterruptDisable(Interrupt interrupt)
		{
			SetInterruptEnable(interrupt, false);
		}
	};
	
	template
		<
			RegisterPointerType RegisterAddr,
			class InterruptEnumClass
		>
	class InterruptFlagRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType OverflowInterruptFlag 				: 1;
			ValueDataType OutputCompareMatchAInterruptFlag	 	: 1;
			ValueDataType OutputCompareMatchBInterruptFlag	 	: 1;
			ValueDataType 			 							: 5;
		};
		
		typedef typename InterruptEnumClass::Interrupt Interrupt;
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static bool GetInterruptFlag(Interrupt interrupt)
		{
			switch(interrupt)
			{
			case InterruptEnumClass::InterruptOverflow:
				return RegRef::_register->OverflowInterruptFlag;
			case InterruptEnumClass::InterruptOutputCompareMatchA:
				return RegRef::_register->OutputCompareMatchAInterruptFlag;
			case InterruptEnumClass::InterruptOutputCompareMatchB:
				return RegRef::_register->OutputCompareMatchBInterruptFlag;
			}
			
			return false;
		}
	};
	
	template
		<
			unsigned int IdObj,
			RegisterPointerType ControlRegisterAAddr,
			RegisterPointerType ControlRegisterBAddr,
			RegisterPointerType CounterRegisterAddr,
			RegisterPointerType OutputCompareARegisterAddr,
			RegisterPointerType OutputCompareBRegisterAddr,
			RegisterPointerType InterruptMaskRegisterAddr,
			RegisterPointerType InterruptFlagRegisterAddr
		>
	class Timer8bitPwm :
		public ControlRegisterA<ControlRegisterAAddr>,
		public ControlRegisterB<ControlRegisterBAddr>,
		public CounterRegister<CounterRegisterAddr>,
		public OutputCompareARegister<OutputCompareARegisterAddr>,
		public OutputCompareBRegister<OutputCompareBRegisterAddr>,
		public InterruptMaskRegister<InterruptMaskRegisterAddr>,
		public InterruptFlagRegister<InterruptFlagRegisterAddr, InterruptMaskRegister<InterruptMaskRegisterAddr> >
	{
	};
}
}
}

#endif