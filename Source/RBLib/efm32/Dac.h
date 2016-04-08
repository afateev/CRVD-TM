#ifndef DAC_EMF32_H
#define DAC_EMF32_H

#include "Register.h"
#include "../IdObjectBase.h"

namespace Rblib
{
namespace Efm32
{
namespace Dac
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// DACn_CTRL - Control Register
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum ConversionMode
		{
			ConversionModeCONTINUOUS	= 0,
			ConversionModeSAMPLEHOLD	= 1,
			ConversionModeSAMPLEOFF		= 2
		};
		
		enum OutputMode
		{
			OutputModeDISABLE	= 0,
			OutputModePIN		= 1,
			OutputModeADC		= 2,
			OutputModePINADC	= 3
		};
		
		enum Reference
		{
			Reference1V25	= 0,
			Reference2V5	= 1,
			ReferenceVdd	= 2
		};
		
		enum RefreshInterval
		{
			RefreshInterval8Cycles		= 0,
			RefreshInterval16Cycles		= 1,
			RefreshInterval32Cycles		= 2,
			RefreshInterval64Cycles		= 3
		};
	protected:
		struct RegStruct
		{
			ValueDataType DifferentialMode				: 1;
			ValueDataType SineMode						: 1;
			ValueDataType ConversionMode				: 2;
			ValueDataType OutputMode					: 2;
			ValueDataType PRSControlledOutputEnable		: 1;
			ValueDataType Channel0StartResetPrescaler	: 1;
			ValueDataType ReferenceSelection			: 2;
			ValueDataType 								: 6;
			ValueDataType PrescalerSetting				: 3;
			ValueDataType 								: 1;
			ValueDataType RefreshIntervalSelect			: 2;
			ValueDataType 								: 10;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableDifferentialMode(bool enable = true)
		{
			Register::Ptr()->DifferentialMode = enable;
		}
		
		static void EnableSineMode(bool enable = true)
		{
			Register::Ptr()->SineMode = enable;
		}
		
		static void SetConversionMode(ConversionMode val)
		{
			Register::Ptr()->ConversionMode = val;
		}
		
		static void SetOutputMode(OutputMode val)
		{
			Register::Ptr()->OutputMode = val;
		}
		
		static void SetReference(Reference val)
		{
			Register::Ptr()->ReferenceSelection = val;
		}
		
		// val - степень двойки
		static void SetPrescaler(unsigned int val)
		{
			Register::Ptr()->PrescalerSetting = val;
		}
		
		static void SetRefreshInterval(RefreshInterval val)
		{
			Register::Ptr()->RefreshIntervalSelection = val;
		}
	};
	
	// DACn_STATUS - Status Register
	template
		<
			RegisterAddressType Address
		>
	class StatusRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Channel0DataValid		: 1;
			ValueDataType Channel1DataValid		: 1;
			ValueDataType 						: 30;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool IsChannel0DataValid()
		{
			return Register::Ptr()->Channel0DataValid;
		}
		
		static bool IsChannel1DataValid()
		{
			return Register::Ptr()->Channel1DataValid;
		}
	};
	
	// DACn_CH0CTRL - Channel 0 Control Register
	template
		<
			RegisterAddressType Address
		>
	class Channel0ControlRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Enable					: 1;
			ValueDataType AutomaticRefreshEnable	: 1;
			ValueDataType PRSTriggerEnable			: 1;
			ValueDataType 							: 1;
			ValueDataType PRSTriggerSelect			: 4;
			ValueDataType 							: 24;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Channel0Enable(bool enable = true)
		{
			Register::Ptr()->Enable = enable;
		}
		
		static void Channel0AutomaticRefreshEnable(bool enable = true)
		{
			Register::Ptr()->AutomaticRefreshEnable = enable;
		}
	};
	
	// DACn_CH1CTRL - Channel 1 Control Register
	template
		<
			RegisterAddressType Address
		>
	class Channel1ControlRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Enable					: 1;
			ValueDataType AutomaticRefreshEnable	: 1;
			ValueDataType PRSTriggerEnable			: 1;
			ValueDataType 							: 1;
			ValueDataType PRSTriggerSelect			: 4;
			ValueDataType 							: 24;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Channel1Enable(bool enable = true)
		{
			Register::Ptr()->Enable = enable;
		}
		
		static void Channel1AutomaticRefreshEnable(bool enable = true)
		{
			Register::Ptr()->AutomaticRefreshEnable = enable;
		}
	};
	
	// DACn_CH0DATA - Channel 0 Data Register
	template
		<
			RegisterAddressType Address
		>
	class Channel0DataRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Data		: 12;
			ValueDataType 			: 20;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Channel0SetData(ValueDataType val)
		{
			Register::Ptr()->Data = val;
		}
	};
	
	// DACn_CH1DATA - Channel 1 Data Register
	template
		<
			RegisterAddressType Address
		>
	class Channel1DataRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType Data		: 12;
			ValueDataType 			: 20;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Channel1SetData(ValueDataType val)
		{
			Register::Ptr()->Data = val;
		}
	};
	
	// DACn_OPA0MUX - Operational Amplifier Mux Configuration Register
	template
		<
			RegisterAddressType Address
		>
	class OperationalAmplifier0MuxConfigurationRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType NonInvertingInputMux		: 3;
			ValueDataType 							: 1;
			ValueDataType InvertingInputMux			: 2;
			ValueDataType 							: 2;
			ValueDataType ResistorLadderInputMux	: 3;
			ValueDataType 							: 1;
			ValueDataType PositivePadInputEnable	: 1;
			ValueDataType NegativePadInputEnable	: 1;
			ValueDataType OutputEnableValue			: 5;
			ValueDataType 							: 3;
			ValueDataType OutputSelect				: 2;
			ValueDataType 							: 2;
			ValueDataType NextEnable				: 1;
			ValueDataType 							: 1;
			ValueDataType ResistorLadderSelect		: 3;
			ValueDataType 							: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Opa0SetOutputEnableMaskBit(unsigned char bit)
		{
			Register::Ptr()->OutputEnableValue |= bit;
		}
		
		static void Opa0ClearOutputEnableMaskBit(unsigned char bit)
		{
			Register::Ptr()->OutputEnableValue &= ~bit;
		}
	};
	
	// DACn_OPA1MUX - Operational Amplifier Mux Configuration Register
	template
		<
			RegisterAddressType Address
		>
	class OperationalAmplifier1MuxConfigurationRegister
	{
	protected:
		struct RegStruct
		{
			ValueDataType NonInvertingInputMux		: 3;
			ValueDataType 							: 1;
			ValueDataType InvertingInputMux			: 2;
			ValueDataType 							: 2;
			ValueDataType ResistorLadderInputMux	: 3;
			ValueDataType 							: 1;
			ValueDataType PositivePadInputEnable	: 1;
			ValueDataType NegativePadInputEnable	: 1;
			ValueDataType OutputEnableValue			: 5;
			ValueDataType 							: 3;
			ValueDataType OutputSelect				: 2;
			ValueDataType 							: 2;
			ValueDataType NextEnable				: 1;
			ValueDataType 							: 1;
			ValueDataType ResistorLadderSelect		: 3;
			ValueDataType 							: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void Opa1SetOutputEnableMaskBit(unsigned char bit)
		{
			Register::Ptr()->OutputEnableValue |= bit;
		}
		
		static void Opa1ClearOutputEnableMaskBit(unsigned char bit)
		{
			Register::Ptr()->OutputEnableValue &= ~bit;
		}
	};
	
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType StatusRegAddr,
			RegisterAddressType Channel0ControlRegAddr,
			RegisterAddressType Channel1ControlRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType Channel0DataRegAddr,
			RegisterAddressType Channel1DataRegAddr,
			RegisterAddressType CombinedDataRegAddr,
			RegisterAddressType CalibrationRegAddr,
			RegisterAddressType BiasProgrammingRegAddr,
			RegisterAddressType OperationalAmplifierControlRegAddr,
			RegisterAddressType OperationalAmplifierOffsetRegAddr,
			RegisterAddressType OperationalAmplifier0MuxConfigurationRegAddr,
			RegisterAddressType OperationalAmplifier1MuxConfigurationRegAddr,
			RegisterAddressType OperationalAmplifier2MuxConfigurationRegAddr
		>
	class Dac :
		public IdObjectBase<IdObj>,
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public StatusRegister<BaseAddr + StatusRegAddr>,
		public Channel0ControlRegister<BaseAddr + Channel0ControlRegAddr>,
		public Channel1ControlRegister<BaseAddr + Channel1ControlRegAddr>,
		public Channel0DataRegister<BaseAddr + Channel0DataRegAddr>,
		public Channel1DataRegister<BaseAddr + Channel1DataRegAddr>,
		public OperationalAmplifier0MuxConfigurationRegister<BaseAddr + OperationalAmplifier0MuxConfigurationRegAddr>,
		public OperationalAmplifier1MuxConfigurationRegister<BaseAddr + OperationalAmplifier1MuxConfigurationRegAddr>
	{
	public:
	};
}
}
}

#endif