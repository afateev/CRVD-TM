#ifndef NVIC_EMF32_H
#define NVIC_EMF32_H

#include "Register.h"

namespace Rblib
{
namespace Efm32
{
namespace Nvic
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// ISER0 - Set Interrupt Enable Register 0
	// ISER1 - Set Interrupt Enable Register 1
	template
		<
			RegisterAddressType Address0,
			RegisterAddressType Address1
		>
	class SetInterruptEnableRegister
	{
	protected:
		typedef Register<Address0, ValueDataType> Register0;
		typedef Register<Address1, ValueDataType> Register1;
	public:
		static void InterruptEnable(unsigned int v)
		{
			volatile ValueDataType *ptr = 0;
			unsigned char offset = 0;
			
			if (v <= 31)
			{
				ptr = Register0::Ptr();
				offset = v;
			}
			
			if (v >= 32 && v <= 63)
			{
				ptr = Register1::Ptr();
				offset = v - 32;
			}
			
			if (ptr)
			{
				*ptr = 1 << offset;
			}
		}
	};
	
	// ISER0 - Set Interrupt Enable Register 0
	// ISER1 - Set Interrupt Enable Register 1
	template
		<
			RegisterAddressType Address0,
			RegisterAddressType Address1
		>
	class ClearInterruptEnableRegister
	{
	protected:
		typedef Register<Address0, ValueDataType> Register0;
		typedef Register<Address1, ValueDataType> Register1;
	public:
		static void InterruptDisable(unsigned int v)
		{
			volatile ValueDataType *ptr = 0;
			unsigned char offset = 0;
			
			if (v <= 31)
			{
				ptr = Register0::Ptr();
				offset = v;
			}
			
			if (v >= 32 && v <= 63)
			{
				ptr = Register1::Ptr();
				offset = v - 32;
			}
			
			if (ptr)
			{
				*ptr = 1 << offset;
			}
		}
	};
	
	// IPR - Interrupt Priority Registers
	template
		<
			RegisterAddressType Address
		>
	class InterruptPriorityRegisters
	{
	protected:
		typedef Register<Address, unsigned char> Register;
	public:
		static unsigned char GetInterruptPriority(unsigned int irqIndex)
		{
			// Only bits[7:5] of each field are implemented, bits[4:0] read as zero and ignore writes.
			// Поэтому сдвиг на 5
			return Register::Ptr()[irqIndex] >> 5;
		}
		
		static void SetInterruptPriority(unsigned int irqIndex, unsigned char priority)
		{
			// Only bits[7:5] of each field are implemented, bits[4:0] read as zero and ignore writes.
			// Поэтому сдвиг на 5
			Register::Ptr()[irqIndex] = (priority & 0x07) << 5;
		}
	};
	
	// STIR - Software Trigger Interrupt Register
	template
		<
			RegisterAddressType Address
		>
	class SoftwareTriggerInterruptRegister
	{
	protected:
		typedef Register<Address, ValueDataType> Register;
	public:
		static void SoftwareTriggerInterrupt(unsigned int irqIndex)
		{
			*Register::Ptr() = irqIndex & 0x1FF;
		}
	};
	
	template
		<
			RegisterAddressType BaseAddr,
			RegisterAddressType SetInterruptEnable0RegAddr,
			RegisterAddressType SetInterruptEnable1RegAddr,
			RegisterAddressType ClearInterruptEnable0RegAddr,
			RegisterAddressType ClearInterruptEnable1RegAddr,
			RegisterAddressType InterruptPriorityRegAddr,
			RegisterAddressType SoftwareTriggerInterruptRegAddr
		>
	class Nvic :
		public SetInterruptEnableRegister<BaseAddr + SetInterruptEnable0RegAddr, BaseAddr + SetInterruptEnable1RegAddr>,
		public ClearInterruptEnableRegister<BaseAddr + ClearInterruptEnable0RegAddr, BaseAddr + ClearInterruptEnable1RegAddr>,
		public InterruptPriorityRegisters<BaseAddr + InterruptPriorityRegAddr>,
		public SoftwareTriggerInterruptRegister<BaseAddr + SoftwareTriggerInterruptRegAddr>
	{
	public:
		enum InterruptVector
		{
			InterruptVector_DMA,
			InterruptVector_GPIO_EVEN,
			InterruptVector_TIMER0,
			InterruptVector_USART0_RX,
			InterruptVector_USART0_TX,
			InterruptVector_USB,
			InterruptVector_ACMP0_ACMP1,
			InterruptVector_ADC0,
			InterruptVector_DAC0,
			InterruptVector_I2C0,
			InterruptVector_I2C1,
			InterruptVector_GPIO_ODD,
			InterruptVector_TIMER1,
			InterruptVector_TIMER2,
			InterruptVector_TIMER3,
			InterruptVector_USART1_RX,
			InterruptVector_USART1_TX,
			InterruptVector_LESENSE,
			InterruptVector_USART2_RX,
			InterruptVector_USART2_TX,
			InterruptVector_UART0_RX,
			InterruptVector_UART0_TX,
			InterruptVector_UART1_RX,
			InterruptVector_UART1_TX,
			InterruptVector_LEUART0,
			InterruptVector_LEUART1,
			InterruptVector_LETIMER0,
			InterruptVector_PCNT0,
			InterruptVector_PCNT1,
			InterruptVector_PCNT2,
			InterruptVector_RTC,
			InterruptVector_BURTC,
			InterruptVector_CMU,
			InterruptVector_VCMP,
			InterruptVector_LCD,
			InterruptVector_MSC,
			InterruptVector_AES,
			InterruptVector_EBI
		};
		
		static const int InterruptVectorCount = 38;
	};
}
}
}

#endif