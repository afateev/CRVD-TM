///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	EMF32
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_EMF32_H
#define GPIO_EMF32_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackListWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace Gpio
{
	// Тип значения порта
	typedef unsigned int ValueDataType;
	// Тип адреса регистра порта
	typedef unsigned int RegisterAddressType;
	
	// GPIO_Px_CTRL - Port Control Register
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum DriveMode
		{
			DriveModeStandard	= 0,
			DriveModeLowest		= 1,
			DriveModeHigh		= 2,
			DriveModeLow		= 3
		};
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void SetDriveMode(DriveMode val)
		{
			*Register::Ptr() = val;
		}
	};
	
	// GPIO_Px_MODEL - Port Pin Mode Low Register
	// GPIO_Px_MODEH - Port Pin Mode High Register
	template
		<
			RegisterAddressType AddressLow,
			RegisterAddressType AddressHi
		>
	class ModeRegister
	{
	public:
		enum Mode
		{
			ModeDisabled					= 0,
			ModeInput						= 1,
			ModeInputPull					= 2,
			ModeInputPullFilter				= 3,
			ModePushPull					= 4,
			ModePushPullDrive				= 5,
			ModeWiredOr						= 6,
			ModeWiredOrPullDown				= 7,
			ModeWiredAnd					= 8,
			ModeWiredAndFilter				= 9,
			ModeWiredAndPullUp				= 10,
			ModeWiredAndPullUpFilter		= 11,
			ModeWiredAndDrive				= 12,
			ModeWiredAndDriveFilter			= 13,
			ModeWiredAndDrivePullUp			= 14,
			ModeWiredAndDrivePullUpFilter	= 15
		};
	protected:
		typedef Register<AddressLow, ValueDataType> RegisterLow;
		typedef Register<AddressHi, ValueDataType> RegisterHi;
	public:
		static void SetMode(unsigned char pin, Mode val)
		{
			volatile ValueDataType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 7)
			{
				ptr = RegisterLow::Ptr();
				offset = pin * 4;
			}
			
			if (pin >= 8 && pin <= 15)
			{
				ptr = RegisterHi::Ptr();
				offset = (pin - 8) * 4;
			}
			
			if (ptr)
			{
				ValueDataType regVal = *ptr;
				regVal &= ~(0x0F << offset);
				regVal |= val << offset;
				
				*ptr = regVal;
			}
		}
	};
	
	// GPIO_Px_DOUT - Port Data Out Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void Write(ValueDataType val)
		{
			*Register::Ptr() = val;
		}
	};
	
	// GPIO_Px_DOUTSET - Port Data Out Set Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutSetRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void SetBit(unsigned char pin)
		{
			*Register::Ptr() = 1 << pin;
		}
	};
	
	// GPIO_Px_DOUTCLR - Port Data Out Clear Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutClearRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ClearBit(unsigned char pin)
		{
			*Register::Ptr() = 1 << pin;
		}
	};
	
	// GPIO_Px_DOUTTGL - Port Data Out Toggle Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutToggleRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ToggleBit(unsigned char pin)
		{
			*Register::Ptr() = 1 << pin;
		}
	};
	
	// GPIO_Px_DIN - Port Data In Register
	template
		<
			RegisterAddressType Address
		>
	class DataInRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static ValueDataType Read()
		{
			return *Register::Ptr();
		}
		
		static ValueDataType Read(unsigned char pin)
		{
			return *Register::Ptr() & (1 << pin);
		}
	};
	
	// GPIO_EXTIPSELL - External Interrupt Port Select Low Register
	// GPIO_EXTIPSELH - External Interrupt Port Select High Register
	template
		<
			RegisterAddressType AddressLow,
			RegisterAddressType AddressHi
		>
	class ExternalInterruptPortSelectRegister
	{
	public:
		enum Port
		{
			PortA					= 0,
			PortB					= 1,
			PortC					= 2,
			PortD					= 3,
			PortE					= 4,
			PortF					= 5
		};
	protected:
		typedef Register<AddressLow, ValueDataType> RegisterLow;
		typedef Register<AddressHi, ValueDataType> RegisterHi;
	public:
		static void ExternalInterruptSetSource(unsigned char interruptLine, Port val)
		{
			volatile ValueDataType *ptr = 0;
			unsigned char offset = 0;
			
			if (interruptLine <= 7)
			{
				ptr = RegisterLow::Ptr();
				offset = interruptLine * 4;
			}
			
			if (interruptLine >= 8 && interruptLine <= 15)
			{
				ptr = RegisterHi::Ptr();
				offset = (interruptLine - 8) * 4;
			}
			
			if (ptr)
			{
				ValueDataType regVal = *ptr;
				regVal &= ~(0x0F << offset);
				regVal |= val << offset;
				
				*ptr = regVal;
			}
		}
		
		static Port ExternalInterruptGetSource(unsigned char interruptLine)
		{
			volatile ValueDataType *ptr = 0;
			unsigned char offset = 0;
			
			if (interruptLine <= 7)
			{
				ptr = RegisterLow::Ptr();
				offset = interruptLine * 4;
			}
			
			if (interruptLine >= 8 && interruptLine <= 15)
			{
				ptr = RegisterHi::Ptr();
				offset = (interruptLine - 8) * 4;
			}
			
			if (ptr)
			{
				ValueDataType regVal = *ptr;
				regVal >>= offset;
				regVal &= 0x0F;
				return (Port)regVal;
			}
			
			return (Port)0;
		}
	};
	
	// GPIO_EXTIRISE - External Interrupt Rising Edge Trigger Register
	template
		<
			RegisterAddressType Address
		>
	class ExternalInterruptRisingEdgeTriggerRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ExternalInterruptRisingEdgeEnable(unsigned char interruptLine)
		{
			*Register::Ptr() |= 1 << interruptLine;
		}
		
		static void ExternalInterruptRisingEdgeDisable(unsigned char interruptLine)
		{
			*Register::Ptr() &= ~(1 << interruptLine);
		}
	};
	
	// GPIO_EXTIFALL - External Interrupt Falling Edge Trigger Register
	template
		<
			RegisterAddressType Address
		>
	class ExternalInterruptFallingEdgeTriggerRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ExternalInterruptFallingEdgeEnable(unsigned char interruptLine)
		{
			*Register::Ptr() |= 1 << interruptLine;
		}
		
		static void ExternalInterruptFallingEdgeDisable(unsigned char interruptLine)
		{
			*Register::Ptr() &= ~(1 << interruptLine);
		}
	};
	
	// GPIO_IEN - Interrupt Enable Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptEnableRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ExternalInterruptEnable(unsigned char interruptLine)
		{
			*Register::Ptr() |= 1 << interruptLine;
		}
		
		static void ExternalInterruptDisable(unsigned char interruptLine)
		{
			*Register::Ptr() &= ~(1 << interruptLine);
		}
	};
	
	// GPIO_IF - Interrupt Flag Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static bool ExternalInterruptFlag(unsigned char interruptLine)
		{
			return *Register::Ptr() & (1 << interruptLine);
		}
	};
	
	// GPIO_IFS - Interrupt Flag Set Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagSetRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ExternalInterruptFlagSet(unsigned char interruptLine)
		{
			*Register::Ptr() = 1 << interruptLine;
		}
	};
	
	// GPIO_IFC - Interrupt Flag Clear Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagClearRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void ExternalInterruptFlagClear(unsigned char interruptLine)
		{
			*Register::Ptr() = 1 << interruptLine;
		}
	};
	
	// Настраиваемые параметры порта
	template
		<
			int IdObj,
			int externalInterruptPortSelectId
		>
	class StaticParams
	{
	public:
		static const int ExternalInterruptPortSelectId = externalInterruptPortSelectId;
		typedef CallbackListWrapper<16, int, bool> HandlerType;
	public:
		static HandlerType PinChangedInterruptHandler;
	};

	template <int IdObj, int externalInterruptPortSelectId>
		StaticParams<IdObj, externalInterruptPortSelectId>::HandlerType StaticParams<IdObj, externalInterruptPortSelectId>::PinChangedInterruptHandler;

	// Шаблон портов
	template
		<
			unsigned int IdObj,
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType ModeLowRegAddr,
			RegisterAddressType ModeHiRegAddr,
			RegisterAddressType DataOutRegAddr,
			RegisterAddressType DataOutSetRegAddr,
			RegisterAddressType DataOutClearRegAddr,
			RegisterAddressType DataOutToggleRegAddr,
			RegisterAddressType DataInRegAddr,
			RegisterAddressType UnlockedPinsRegAddr,
			RegisterAddressType ExternalInterruptPortSelectLowRegAddr,
			RegisterAddressType ExternalInterruptPortSelectHighRegAddr,
			RegisterAddressType ExternalInterruptRisingEdgeTriggerRegAddr,
			RegisterAddressType ExternalInterruptFallingEdgeTriggerRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			int externalInterruptPortSelectId
		>
	class Port :
		public IdObjectBase<IdObj>,
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public ModeRegister<BaseAddr + ModeLowRegAddr, BaseAddr + ModeHiRegAddr>,
		public DataOutRegister<BaseAddr + DataOutRegAddr>,
		public DataOutSetRegister<BaseAddr + DataOutSetRegAddr>,
		public DataOutClearRegister<BaseAddr + DataOutClearRegAddr>,
		public DataOutToggleRegister<BaseAddr + DataOutToggleRegAddr>,
		public DataInRegister<BaseAddr + DataInRegAddr>,
		public ExternalInterruptPortSelectRegister<BaseAddr + ExternalInterruptPortSelectLowRegAddr, BaseAddr + ExternalInterruptPortSelectHighRegAddr>,
		public ExternalInterruptRisingEdgeTriggerRegister<BaseAddr + ExternalInterruptRisingEdgeTriggerRegAddr>,
		public ExternalInterruptFallingEdgeTriggerRegister<BaseAddr + ExternalInterruptFallingEdgeTriggerRegAddr>,
		public InterruptEnableRegister<BaseAddr + InterruptEnableRegAddr>,
		public InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr>,
		public InterruptFlagSetRegister<BaseAddr + InterruptFlagSetRegAddr>,
		public InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr>,
		public StaticParams<IdObj, externalInterruptPortSelectId>
	{
	public:
		typedef DataInRegister<BaseAddr + DataInRegAddr> DataInRegister;
		typedef StaticParams<IdObj, externalInterruptPortSelectId> StaticParams;
	public:
		static void InterruptHandler(int line)
		{
			StaticParams::PinChangedInterruptHandler.Call(line, DataInRegister::Read(line));
		}
	};
	
	template
		<
			RegisterAddressType BaseAddr,
			RegisterAddressType ExternalInterruptPortSelectLowRegAddr,
			RegisterAddressType ExternalInterruptPortSelectHighRegAddr,
			RegisterAddressType ExternalInterruptRisingEdgeTriggerRegAddr,
			RegisterAddressType ExternalInterruptFallingEdgeTriggerRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr
		>
	class InterruptHandlerImpl :
		public ExternalInterruptPortSelectRegister<BaseAddr + ExternalInterruptPortSelectLowRegAddr, BaseAddr + ExternalInterruptPortSelectHighRegAddr>,
		public ExternalInterruptRisingEdgeTriggerRegister<BaseAddr + ExternalInterruptRisingEdgeTriggerRegAddr>,
		public ExternalInterruptFallingEdgeTriggerRegister<BaseAddr + ExternalInterruptFallingEdgeTriggerRegAddr>,
		public InterruptEnableRegister<BaseAddr + InterruptEnableRegAddr>,
		public InterruptFlagRegister<BaseAddr + InterruptFlagRegAddr>,
		public InterruptFlagSetRegister<BaseAddr + InterruptFlagSetRegAddr>,
		public InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr>
	{
	};
}
}
}

#endif