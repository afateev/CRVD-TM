#ifndef NVIC_STM32_H
#define NVIC_STM32_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace NvicImplementation
{
	// Interrupt Set-Enable Registers
	class InterruptSetEnableRegisters
	{
	public:
		static const Core::RegisterAddressType Address = 0xE000E100;
		static const int MaxCount = 28;
	protected:
		typedef Register<Address, Core::RegisterAddressType> Register;
	public:
		static void InterruptEnable(unsigned int v)
		{
			unsigned int regNumber = v / 32;
			
			if (regNumber >= MaxCount)
			{
				return;
			}
			
			Core::RegisterAddressType regOffset = sizeof(Core::RegisterValueType) * regNumber;
			
			volatile Core::RegisterValueType *ptr = Register::Ptr(regOffset);
			unsigned char offset = v % 32;
			if (ptr)
			{
				*ptr = 1 << offset;
			}
		}
	};
	
	// Interrupt Clear-Enable Registers
	class InterruptClearEnableRegisters
	{
	public:
		static const Core::RegisterAddressType Address = 0xE000E180;
		static const int MaxCount = 28;
	protected:
		typedef Register<Address, Core::RegisterAddressType> Register;
	public:
		static void InterruptDisable(unsigned int v)
		{
			unsigned int regNumber = v / 32;
			
			if (regNumber >= MaxCount)
			{
				return;
			}
			
			Core::RegisterAddressType regOffset = sizeof(Core::RegisterValueType) * regNumber;
			
			volatile Core::RegisterValueType *ptr = Register::Ptr(regOffset);
			unsigned char offset = v % 32;
			if (ptr)
			{
				*ptr = 1 << offset;
			}
		}
	};
	
	// Interrupt Priority Registers
	class InterruptPriorityRegisters
	{
	public:
		static const Core::RegisterAddressType Address = 0xE000E400;
		static const int MaxCount = 31;
	protected:
		typedef Register<Address, unsigned char> Register;
	public:
		static unsigned char GetInterruptPriority(unsigned int irqIndex)
		{
			return Register::Ptr()[irqIndex];
		}
		
		static void SetInterruptPriority(unsigned int irqIndex, unsigned char priority)
		{
			Register::Ptr()[irqIndex] = priority;
		}
	};
	
	class Nvic :
		public InterruptSetEnableRegisters,
		public InterruptClearEnableRegisters,
		public InterruptPriorityRegisters
	{
	public:
		enum InterruptVector
		{
			InterruptVector_WWDG		=	0,
			InterruptVector_PVD,
			InterruptVector_TAMPER,
			InterruptVector_RTC,
			InterruptVector_FLASH,
			InterruptVector_RCC,
			InterruptVector_EXTI0,
			InterruptVector_EXTI1,
			InterruptVector_EXTI2,
			InterruptVector_EXTI3,
			InterruptVector_EXTI4,
			InterruptVector_DMA1_Channel1,
			InterruptVector_DMA1_Channel2,
			InterruptVector_DMA1_Channel3,
			InterruptVector_DMA1_Channel4,
			InterruptVector_DMA1_Channel5,
			InterruptVector_DMA1_Channel6,
			InterruptVector_DMA1_Channel7,
			InterruptVector_ADC1_2,
			InterruptVector_CAN1_TX,
			InterruptVector_CAN1_RX0,
			InterruptVector_CAN1_RX1,
			InterruptVector_CAN1_SCE,
			InterruptVector_EXTI9_5,
			InterruptVector_TIM1_BRK,
			InterruptVector_TIM1_UP,
			InterruptVector_TIM1_TRG_COM,
			InterruptVector_TIM1_CC,
			InterruptVector_TIM2,
			InterruptVector_TIM3,
			InterruptVector_TIM4,
			InterruptVector_I2C1_EV,
			InterruptVector_I2C1_ER,
			InterruptVector_I2C2_EV,
			InterruptVector_I2C2_ER,
			InterruptVector_SPI1,
			InterruptVector_SPI2,
			InterruptVector_USART1,
			InterruptVector_USART2,
			InterruptVector_USART3,
			InterruptVector_EXTI15_10,
			InterruptVector_RTCAlarm,
			InterruptVector_OTG_FS_WKUP,
			InterruptVector_TIM8_BRK_TIM12,
			InterruptVector_TIM8_UP_TIM13,
			InterruptVector_TIM8_TRG_COM_TIM14,
			InterruptVector_TIM8_CC,
			InterruptVector_ADC3,
			InterruptVector_FSMC,
			InterruptVector_SDIO,
			InterruptVector_TIM5,
			InterruptVector_SPI3,
			InterruptVector_UART4,
			InterruptVector_UART5,
			InterruptVector_TIM6,
			InterruptVector_TIM7,
			InterruptVector_DMA2_Channel1,
			InterruptVector_DMA2_Channel2,
			InterruptVector_DMA2_Channel3,
			InterruptVector_DMA2_Channel4,
			InterruptVector_DMA2_Channel5,
			InterruptVector_ETH,
			InterruptVector_ETH_WKUP,
			InterruptVector_CAN2_TX,
			InterruptVector_CAN2_RX0,
			InterruptVector_CAN2_RX1,
			InterruptVector_CAN2_SCE,
			InterruptVector_OTG_FS,
		};
	};
}

typedef NvicImplementation::Nvic Nvic;
}
}

#endif