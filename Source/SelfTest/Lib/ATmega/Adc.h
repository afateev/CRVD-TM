///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ модулем ј÷ѕ
//	ATMEGA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_ATMEGA_H
#define ADC_ATMEGA_H

#include "../IdObjectBase.h"
#include "../RegisterReference.h"

namespace Lib
{
namespace ATmega
{
namespace Adc
{
	// “ип значени€ порта
	typedef unsigned char ValueDataType;
	
	// ADMUX Ц ADC Multiplexer Selection Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class MultiplexerSelectionRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType AnalogChannelAndGainSelection 	: 5;
			ValueDataType LeftAdjustResult					: 1;
			ValueDataType ReferenceSelection 				: 2;
		};
		
		enum AnalogChannel
		{
			AnalogChannel0	= 0,
			AnalogChannel1	= 1,
			AnalogChannel2	= 2,
			AnalogChannel3	= 3,
			AnalogChannel4	= 4,
			AnalogChannel5	= 5,
			AnalogChannel6	= 6,
			AnalogChannel7	= 7,
		};
		
		enum Reference
		{
			ReferenceAREF	= 0,
			ReferenceAVCC	= 1,
			ReferenceInternal1_1V	= 2,
			ReferenceInternal2_56V	= 3,
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetAnalogChannel(AnalogChannel ch)
		{
			AnalogChannel current = (AnalogChannel)RegRef::_register->AnalogChannelAndGainSelection;
			bool wait = current != ch;
			RegRef::_register->AnalogChannelAndGainSelection = ch;
			if (wait)
			{
				// надо подождать после переключени€
				for (unsigned int i = 0; i < 10000; i++)
				{
					;
				}
			}
		}
		
		static void SetReference(Reference val)
		{
			RegRef::_register->ReferenceSelection = val;
		}
	};
	
	// ADCSRA Ц ADC Control and Status Register A
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlAndStatusRegisterA
	{
	public:
		struct RegisterStruct
		{
			ValueDataType PrescalerSelect 			: 3;
			ValueDataType InterruptEnable			: 1;
			ValueDataType InterruptFlag 			: 1;
			ValueDataType AutoTriggerEnable 		: 1;
			ValueDataType StartConversion 			: 1;
			ValueDataType Enable 					: 1;
		};
		
		enum Divider
		{
			Divider2	= 1,
			Divider4	= 2,
			Divider8	= 3,
			Divider16	= 4,
			Divider32	= 5,
			Divider64	= 6,
			Divider128	= 7,
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetPrescaler(Divider val)
		{
			RegRef::_register->PrescalerSelect = val;
		}
		
		static void SetInterruptEnable(bool val)
		{
			RegRef::_register->InterruptEnable = val;
		}
		
		static bool IsInterruptFlag()
		{
			return RegRef::_register->InterruptFlag;
		}
		
		static void ClearInterruptFlag()
		{
			RegRef::_register->InterruptFlag = true;
		}
		
		static void AutoTriggerEnable(bool enable)
		{
			RegRef::_register->AutoTriggerEnable = enable;
		}
		
		static void StartConversion()
		{
			RegRef::_register->StartConversion = true;
		}
		
		static bool IsConversionProgress()
		{
			return RegRef::_register->StartConversion;
		}
		
		static void Enable(bool enable)
		{
			RegRef::_register->Enable = enable;
		}
	};
	
	// ADCL Ц The ADC Data Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class DataLRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static unsigned char GetDataL()
		{
			return *RegRef::_register;
		}
	};
	
	// ADCH Ц The ADC Data Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class DataHRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static unsigned char GetDataH()
		{
			return *RegRef::_register;
		}
	};
	
	// ADCSRB Ц ADC Control and Status Register B
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlAndStatusRegisterB
	{
	public:
		struct RegisterStruct
		{
			ValueDataType AutoTriggerSource 					: 3;
			ValueDataType 										: 3;
			ValueDataType AnalogComparatorMultiplexerEnable 	: 1;
			ValueDataType  										: 1;
		};
		
		enum TriggerSource
		{
			TriggerSourceFreeRunningMode			= 0,
			TriggerSourceAnalogComparator			= 1,
			TriggerSourceExternalInterruptRequest0	= 2,
			TriggerSourceTimer0CompareMatch			= 3,
			TriggerSourceTimer0Overflow				= 4,
			TriggerSourceTimer1CompareMatchB		= 5,
			TriggerSourceTimer1Overflow				= 6,
			TriggerSourceTimer1CaptureEvent			= 7,
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetAutoTriggerSource(TriggerSource val)
		{
			RegRef::_register->AutoTriggerSource = val;
		}
	};
	
	// DIDR0 Ц Digital Input Disable Register 0
	template
		<
			RegisterPointerType RegisterAddr
		>
	class DigitalInputDisableRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void DisableInput(unsigned char pin)
		{
			*RegRef::_register |= (1 << pin);
		}
		
		static void EnableInput(unsigned char pin)
		{
			*RegRef::_register &= ~(1 << pin);
		}
	};

	template
		<
			unsigned int IdObj,
			RegisterPointerType MultiplexerSelectionRegisterAddr,
			RegisterPointerType ControlAndStatusRegisterAAddr,
			RegisterPointerType DataLRegisterAddr,
			RegisterPointerType DataHRegisterAddr,
			RegisterPointerType ControlAndStatusRegisterBAddr,
			RegisterPointerType DigitalInputDisableRegisterAddr
		>
	class Adc :
		public MultiplexerSelectionRegister<MultiplexerSelectionRegisterAddr>,
		public ControlAndStatusRegisterA<ControlAndStatusRegisterAAddr>,
		public DataLRegister<DataLRegisterAddr>,
		public DataHRegister<DataHRegisterAddr>,
		public ControlAndStatusRegisterB<ControlAndStatusRegisterBAddr>,
		public DigitalInputDisableRegister<DigitalInputDisableRegisterAddr>
	{
	public:
		typedef MultiplexerSelectionRegister<MultiplexerSelectionRegisterAddr> MultiplexerSelectionRegister;
		typedef ControlAndStatusRegisterA<ControlAndStatusRegisterAAddr> ControlAndStatusRegisterA;
		typedef DataLRegister<DataLRegisterAddr> DataLRegister;
		typedef DataHRegister<DataHRegisterAddr> DataHRegister;
	public:
		static void Enable(typename ControlAndStatusRegisterA::Divider val, typename MultiplexerSelectionRegister::Reference ref)
		{
			ControlAndStatusRegisterA::SetPrescaler(val);
			MultiplexerSelectionRegister::SetReference(ref);
			ControlAndStatusRegisterA::Enable(true);
		}
		
		static void Disable()
		{
			ControlAndStatusRegisterA::Enable(false);
		}
		
		static unsigned short DoConversion(typename MultiplexerSelectionRegister::AnalogChannel ch)
		{
			MultiplexerSelectionRegister::SetAnalogChannel(ch);
			ControlAndStatusRegisterA::ClearInterruptFlag();
			ControlAndStatusRegisterA::StartConversion();
			while (ControlAndStatusRegisterA::IsConversionProgress())
			//while (!ControlAndStatusRegisterA::IsInterruptFlag())
			{
				;
			}
			
			unsigned short val = DataLRegister::GetDataL();
			val += DataHRegister::GetDataH() << 8;
			
			return val;
		}
	};
}
}
}

#endif