///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ портами ввода-вывода
//	ATMEGA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_ATMEGA_H
#define GPIO_ATMEGA_H

#include "../IdObjectBase.h"

namespace Lib
{
namespace ATmega
{
namespace Gpio
{
	// “ип значени€ порта
	typedef unsigned char ValueDataType;
	// “ип адреса регистра порта
	typedef unsigned int RegisterAddressType;
	/*
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
		static volatile ValueDataType * const _regPtr;
	public:
		static void SetDriveMode(DriveMode val)
		{
			*_regPtr = val;
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const ControlRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
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
		static volatile ValueDataType * const _regLowPtr;
		static volatile ValueDataType * const _regHiPtr;
	public:
		static void SetMode(unsigned char pin, Mode val)
		{
			volatile ValueDataType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 7)
			{
				ptr = _regLowPtr;
				offset = pin * 4;
			}
			
			if (pin >= 8 && pin <= 15)
			{
				ptr = _regHiPtr;
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
	
	template<RegisterAddressType AddressLow, RegisterAddressType AddressHi>
		volatile ValueDataType * const ModeRegister<AddressLow, AddressHi>::_regLowPtr = reinterpret_cast<volatile ValueDataType*>(AddressLow);
	template<RegisterAddressType AddressLow, RegisterAddressType AddressHi>
		volatile ValueDataType * const ModeRegister<AddressLow, AddressHi>::_regHiPtr = reinterpret_cast<volatile ValueDataType*>(AddressHi);
	
	// GPIO_Px_DOUT - Port Data Out Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void Write(ValueDataType val)
		{
			*_regPtr = val;
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const DataOutRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_Px_DOUTSET - Port Data Out Set Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutSetRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void SetBit(unsigned char pin)
		{
			*_regPtr = 1 << pin;
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const DataOutSetRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_Px_DOUTCLR - Port Data Out Clear Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutClearRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void ClearBit(unsigned char pin)
		{
			*_regPtr = 1 << pin;
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const DataOutClearRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_Px_DOUTTGL - Port Data Out Toggle Register
	template
		<
			RegisterAddressType Address
		>
	class DataOutToggleRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void ToggleBit(unsigned char pin)
		{
			*_regPtr = 1 << pin;
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const DataOutToggleRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_Px_DIN - Port Data In Register
	template
		<
			RegisterAddressType Address
		>
	class DataInRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static ValueDataType Read()
		{
			return *_regPtr;
		}
		
		static ValueDataType Read(unsigned char pin)
		{
			return *_regPtr & (1 << pin);
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const DataInRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
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
		static volatile ValueDataType * const _regLowPtr;
		static volatile ValueDataType * const _regHiPtr;
	public:
		static void ExternalInterruptSetSource(unsigned char interruptLine, Port val)
		{
			volatile ValueDataType *ptr = 0;
			unsigned char offset = 0;
			
			if (interruptLine <= 7)
			{
				ptr = _regLowPtr;
				offset = interruptLine * 4;
			}
			
			if (interruptLine >= 8 && interruptLine <= 15)
			{
				ptr = _regHiPtr;
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
				ptr = _regLowPtr;
				offset = interruptLine * 4;
			}
			
			if (interruptLine >= 8 && interruptLine <= 15)
			{
				ptr = _regHiPtr;
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
	
	template<RegisterAddressType AddressLow, RegisterAddressType AddressHi>
		volatile ValueDataType * const ExternalInterruptPortSelectRegister<AddressLow, AddressHi>::_regLowPtr = reinterpret_cast<volatile ValueDataType*>(AddressLow);
	template<RegisterAddressType AddressLow, RegisterAddressType AddressHi>
		volatile ValueDataType * const ExternalInterruptPortSelectRegister<AddressLow, AddressHi>::_regHiPtr = reinterpret_cast<volatile ValueDataType*>(AddressHi);
	
	// GPIO_EXTIRISE - External Interrupt Rising Edge Trigger Register
	template
		<
			RegisterAddressType Address
		>
	class ExternalInterruptRisingEdgeTriggerRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void ExternalInterruptRisingEdgeEnable(unsigned char interruptLine)
		{
			*_regPtr |= 1 << interruptLine;
		}
		
		static void ExternalInterruptRisingEdgeDisable(unsigned char interruptLine)
		{
			*_regPtr &= ~(1 << interruptLine);
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const ExternalInterruptRisingEdgeTriggerRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_EXTIFALL - External Interrupt Falling Edge Trigger Register
	template
		<
			RegisterAddressType Address
		>
	class ExternalInterruptFallingEdgeTriggerRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void ExternalInterruptFallingEdgeEnable(unsigned char interruptLine)
		{
			*_regPtr |= 1 << interruptLine;
		}
		
		static void ExternalInterruptFallingEdgeDisable(unsigned char interruptLine)
		{
			*_regPtr &= ~(1 << interruptLine);
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const ExternalInterruptFallingEdgeTriggerRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_IEN - Interrupt Enable Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptEnableRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void ExternalInterruptEnable(unsigned char interruptLine)
		{
			*_regPtr |= 1 << interruptLine;
		}
		
		static void ExternalInterruptDisable(unsigned char interruptLine)
		{
			*_regPtr &= ~(1 << interruptLine);
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const InterruptEnableRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_IF - Interrupt Flag Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static bool ExternalInterruptFlag(unsigned char interruptLine)
		{
			return *_regPtr & (1 << interruptLine);
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const InterruptFlagRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_IFS - Interrupt Flag Set Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagSetRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void ExternalInterruptFlagSet(unsigned char interruptLine)
		{
			*_regPtr = 1 << interruptLine;
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const InterruptFlagSetRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// GPIO_IFC - Interrupt Flag Clear Register
	template
		<
			RegisterAddressType Address
		>
	class InterruptFlagClearRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void ExternalInterruptFlagClear(unsigned char interruptLine)
		{
			*_regPtr = 1 << interruptLine;
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const InterruptFlagClearRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// Ўаблон портов
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
			RegisterAddressType InterruptFlagClearRegAddr
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
		public InterruptFlagClearRegister<BaseAddr + InterruptFlagClearRegAddr>
	{
	};
	*/
	
	// PORTxЦ Port x Data Register
	template
		<
			RegisterAddressType Address
		>
	class DataRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void Write(ValueDataType val)
		{
			*_regPtr = val;
		}
		
		static void SetBit(unsigned char pin)
		{
			*_regPtr |= (1 << pin);
		}
		
		static void ClearBit(unsigned char pin)
		{
			*_regPtr &= ~(1 << pin);
		}
		
		static void PullUpEnable(unsigned char pin)
		{
			SetBit(pin);
		}
		
		static void PullUpDisable(unsigned char pin)
		{
			ClearBit(pin);
		}
		
		static void ToggleBit(unsigned char pin)
		{
			*_regPtr ^= (1 << pin);
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const DataRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// DDRx Ц Port x Data Direction Register
	template
		<
			RegisterAddressType Address
		>
	class DataDirectionRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static void SetDirection(ValueDataType mask)
		{
			*_regPtr = mask;
		}
		
		static void SetOutputPin(unsigned char pin)
		{
			*_regPtr |= (1 << pin);
		}
		
		static void SetInputPin(unsigned char pin)
		{
			*_regPtr &= ~(1 << pin);
		}
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const DataDirectionRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// DDRx Ц Port x Data Direction Register
	template
		<
			RegisterAddressType Address
		>
	class InputPinsAddressRegister
	{
	protected:
		static volatile ValueDataType * const _regPtr;
	public:
		static ValueDataType Read()
		{
			return *_regPtr;
		}
		
		static ValueDataType Read(unsigned char pin)
		{
			return *_regPtr & (1 << pin);
		}
		/*
		static void ToggleBit(unsigned char pin)
		{
			*_regPtr |= (1 << pin);
		}*/
	};
	
	template<RegisterAddressType Address>
		volatile ValueDataType * const InputPinsAddressRegister<Address>::_regPtr = reinterpret_cast<volatile ValueDataType*>(Address);
	
	// Ўаблон портов
	template
		<
			unsigned int IdObj,
			RegisterAddressType DataRegisterAddr,
			RegisterAddressType DataDirectionRegisterAddr,
			RegisterAddressType InputPinsAddressRegisterAddr
		>
	class Port :
		public IdObjectBase<IdObj>,
		public DataRegister<DataRegisterAddr>,
		public DataDirectionRegister<DataDirectionRegisterAddr>,
		public InputPinsAddressRegister<InputPinsAddressRegisterAddr>
	{
	};
}
}
}

#endif