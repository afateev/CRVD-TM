#ifndef RCC_STM32_H
#define RCC_STM32_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace ResetAndClockControlImplementation
{
	static const unsigned int BaseAddress = 0x40021000;
	
	// Clock control register (RCC_CR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ClockControlRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType HSION									: 1;
			Core::RegisterValueType HSIRDY									: 1;
			Core::RegisterValueType 										: 1;
			Core::RegisterValueType HSITRIM									: 5;
			Core::RegisterValueType HSICAL									: 8;
			Core::RegisterValueType HSEON									: 1;
			Core::RegisterValueType HSERDY									: 1;
			Core::RegisterValueType HSEBYP									: 1;
			Core::RegisterValueType CSSON									: 1;
			Core::RegisterValueType 										: 4;
			Core::RegisterValueType PLLON									: 1;
			Core::RegisterValueType PLLRDY									: 1;
			Core::RegisterValueType											: 6;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void HSIEnable(bool enable = true)
		{
			Register::Ptr()->HSION = enable;
		}
		
		static bool HSIReady()
		{
			return Register::Ptr()->HSIRDY;
		}
		
		static void HSEEnable(bool enable = true)
		{
			Register::Ptr()->HSEON = enable;
		}
		
		static bool HSEReady()
		{
			return Register::Ptr()->HSERDY;
		}
		
		static void PLLEnable(bool enable = true)
		{
			Register::Ptr()->PLLON = enable;
		}
		
		static bool PLLReady()
		{
			return Register::Ptr()->PLLRDY;
		}
	};
	
	// Clock configuration register (RCC_CFGR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ClockConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x04;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum SystemClock
		{
			SystemClockHSI		= 0,
			SystemClockHSE		= 1,
			SystemClockPLL		= 2
		};
		
		enum AHBPrescaler
		{
			AHBPrescalerSysClk			= 0,
			AHBPrescalerSysClkDiv2		= 8,
			AHBPrescalerSysClkDiv4		= 9,
			AHBPrescalerSysClkDiv8		= 10,
			AHBPrescalerSysClkDiv16		= 11,
			AHBPrescalerSysClkDiv64		= 12,
			AHBPrescalerSysClkDiv128	= 13,
			AHBPrescalerSysClkDiv256	= 14,
			AHBPrescalerSysClkDiv512	= 15
		};
		
		enum APB1Prescaler
		{
			APB1PrescalerHCLK			= 0,
			APB1PrescalerHCLKDiv2		= 4,
			APB1PrescalerHCLKDiv4		= 5,
			APB1PrescalerHCLKDiv8		= 6,
			APB1PrescalerHCLKDiv16		= 7,
		};
		
		enum APB2Prescaler
		{
			APB2PrescalerHCLK			= 0,
			APB2PrescalerHCLKDiv2		= 4,
			APB2PrescalerHCLKDiv4		= 5,
			APB2PrescalerHCLKDiv8		= 6,
			APB2PrescalerHCLKDiv16		= 7,
		};
		
		enum Adc2Prescaler
		{
			Adc2PrescalerPCLK2Div2		= 0,
			Adc2PrescalerPCLK2Div4		= 1,
			Adc2PrescalerPCLK2Div6		= 2,
			Adc2PrescalerPCLK2Div8		= 3,
		};
		
		enum PLLSource
		{
			PLLSourceHSIDiv2	= 0,
			PLLSourceHSE		= 1,
		};
		
		enum PLLHSEDivider
		{
			HSEDiv1		= 0,
			HSEDiv2		= 1,
		};
		
		enum PLLMultiplicationFactor
		{
			PLLMul2		= 0,
			PLLMul3		= 1,
			PLLMul4		= 2,
			PLLMul5		= 3,
			PLLMul6		= 4,
			PLLMul7		= 5,
			PLLMul8		= 6,
			PLLMul9		= 7,
			PLLMul10	= 8,
			PLLMul11	= 9,
			PLLMul12	= 1,
			PLLMul13	= 11,
			PLLMul14	= 12,
			PLLMul15	= 13,
			PLLMul16	= 14,
		};
		
		enum UsbPrescaler
		{
			UsbPrescalerDiv1point5		= 0,
			UsbPrescalerDiv1			= 1,
		};
		
		enum ClockOutput
		{
			ClockOutputNone		= 0,
			ClockOutputSysClk	= 4,
			ClockOutputHSI		= 5,
			ClockOutputHSE		= 6,
			ClockOutputPllDiv2	= 7
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType SW										: 2;
			Core::RegisterValueType SWS										: 2;
			Core::RegisterValueType HPRE									: 4;
			Core::RegisterValueType PPRE1									: 3;
			Core::RegisterValueType PPRE2									: 3;
			Core::RegisterValueType ADCPRE									: 2;
			Core::RegisterValueType PLLSRC									: 1;
			Core::RegisterValueType PLLXTPRE								: 1;
			Core::RegisterValueType PLLMUL									: 4;
			Core::RegisterValueType USBPRE									: 1;
			Core::RegisterValueType 										: 1;
			Core::RegisterValueType MCO										: 3;
			Core::RegisterValueType 										: 5;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetSystemClock(SystemClock systemClock)
		{
			Register::Ptr()->SW = systemClock;
		}
		
		static void SetAHBPrescaler(AHBPrescaler prescaler)
		{
			Register::Ptr()->HPRE = prescaler;
		}
		
		static void SetAPB1Prescaler(APB1Prescaler prescaler)
		{
			Register::Ptr()->PPRE1 = prescaler;
		}
		
		static void SetAPB2Prescaler(APB2Prescaler prescaler)
		{
			Register::Ptr()->PPRE2 = prescaler;
		}
		
		static void SetAdc2Prescaler(Adc2Prescaler prescaler)
		{
			Register::Ptr()->ADCPRE = prescaler;
		}
		
		static void SetPLLSource(PLLSource source)
		{
			Register::Ptr()->PLLSRC = source;
		}
		
		static void SetPLLHSEDivider(PLLHSEDivider divider)
		{
			Register::Ptr()->PLLXTPRE = divider;
		}
		
		static void SetPLLMultiplicationFactor(PLLMultiplicationFactor mul)
		{
			Register::Ptr()->PLLMUL = mul;
		}
		
		static void SetUsbPrescaler(UsbPrescaler prescaler)
		{
			Register::Ptr()->USBPRE = prescaler;
		}
		
		static void SetClockOutput(ClockOutput clockOutput)
		{
			Register::Ptr()->MCO = clockOutput;
		}
	};
	
	// AHB peripheral clock enable register (RCC_AHBENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AHBPeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x14;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType DMA1EN			: 1;
			Core::RegisterValueType DMA2EN			: 1;
			Core::RegisterValueType SRAMEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType FLITFEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType CRCEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType FSMCEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType SDIOEN			: 1;
			Core::RegisterValueType 				: 21;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockDma1(bool enable = true)
		{
			Register::Ptr()->DMA1EN = enable;
		}
		
		static void EnableClockDma2(bool enable = true)
		{
			Register::Ptr()->DMA2EN = enable;
		}
	};
	
	// APB2 peripheral clock enable register (RCC_APB2ENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class APB2PeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x18;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType AFIOEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType IOPAEN			: 1;
			Core::RegisterValueType IOPBEN			: 1;
			Core::RegisterValueType IOPCEN			: 1;
			Core::RegisterValueType IOPDEN			: 1;
			Core::RegisterValueType IOPEEN			: 1;
			Core::RegisterValueType IOPFEN			: 1;
			Core::RegisterValueType IOPGEN			: 1;
			Core::RegisterValueType ADC1EN			: 1;
			Core::RegisterValueType ADC2EN			: 1;
			Core::RegisterValueType TIM1EN			: 1;
			Core::RegisterValueType SPI1EN			: 1;
			Core::RegisterValueType TIM8EN			: 1;
			Core::RegisterValueType USART1EN		: 1;
			Core::RegisterValueType ADC3EN			: 1;
			Core::RegisterValueType 				: 3;
			Core::RegisterValueType TIM9EN			: 1;
			Core::RegisterValueType TIM10EN			: 1;
			Core::RegisterValueType TIM11EN			: 1;
			Core::RegisterValueType 				: 10;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockAfio(bool enable = true)
		{
			Register::Ptr()->AFIOEN = enable;
		}
		
		static void EnableClockPortA(bool enable = true)
		{
			Register::Ptr()->IOPAEN = enable;
		}
		
		static void EnableClockPortB(bool enable = true)
		{
			Register::Ptr()->IOPBEN = enable;
		}
		
		static void EnableClockPortC(bool enable = true)
		{
			Register::Ptr()->IOPCEN = enable;
		}
		
		static void EnableClockPortD(bool enable = true)
		{
			Register::Ptr()->IOPDEN = enable;
		}
		
		static void EnableClockPortE(bool enable = true)
		{
			Register::Ptr()->IOPEEN = enable;
		}
		
		static void EnableClockPortF(bool enable = true)
		{
			Register::Ptr()->IOPFEN = enable;
		}
		
		static void EnableClockPortG(bool enable = true)
		{
			Register::Ptr()->IOPGEN = enable;
		}
		
		static void EnableClockAdc1(bool enable = true)
		{
			Register::Ptr()->ADC1EN = enable;
		}
		
		static void EnableClockAdc2(bool enable = true)
		{
			Register::Ptr()->ADC2EN = enable;
		}
		
		static void EnableClockTimer1(bool enable = true)
		{
			Register::Ptr()->TIM1EN = enable;
		}
		
		static void EnableClockSpi1(bool enable = true)
		{
			Register::Ptr()->SPI1EN = enable;
		}
		
		static void EnableClockTimer8(bool enable = true)
		{
			Register::Ptr()->TIM8EN = enable;
		}
		
		static void EnableClockUsart1(bool enable = true)
		{
			Register::Ptr()->USART1EN = enable;
		}
		
		static void EnableClockAdc3(bool enable = true)
		{
			Register::Ptr()->ADC3EN = enable;
		}
		
		static void EnableClockTimer9(bool enable = true)
		{
			Register::Ptr()->TIM9EN = enable;
		}
		
		static void EnableClockTimer10(bool enable = true)
		{
			Register::Ptr()->TIM10EN = enable;
		}
		
		static void EnableClockTimer11(bool enable = true)
		{
			Register::Ptr()->TIM11EN = enable;
		}
	};
	
	// APB1 peripheral clock enable register (RCC_APB1ENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class APB1PeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x1C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType TIM2EN			: 1;
			Core::RegisterValueType TIM3EN			: 1;
			Core::RegisterValueType TIM4EN			: 1;
			Core::RegisterValueType TIM5EN			: 1;
			Core::RegisterValueType TIM6EN			: 1;
			Core::RegisterValueType TIM7EN			: 1;
			Core::RegisterValueType TIM12EN			: 1;
			Core::RegisterValueType TIM13EN			: 1;
			Core::RegisterValueType TIM14EN			: 1;
			Core::RegisterValueType 				: 2;
			Core::RegisterValueType WWDGEN			: 1;
			Core::RegisterValueType 				: 2;
			Core::RegisterValueType SPI2EN			: 1;
			Core::RegisterValueType SPI3EN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType USART2EN		: 1;
			Core::RegisterValueType USART3EN		: 1;
			Core::RegisterValueType UART4EN			: 1;
			Core::RegisterValueType UART5EN			: 1;
			Core::RegisterValueType I2C1EN			: 1;
			Core::RegisterValueType I2C2EN			: 1;
			Core::RegisterValueType USBEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType CANEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType BKPEN			: 1;
			Core::RegisterValueType PWREN			: 1;
			Core::RegisterValueType DACEN			: 1;
			Core::RegisterValueType 				: 2;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockTimer2(bool enable = true)
		{
			Register::Ptr()->TIM2EN = enable;
		}
		
		static void EnableClockTimer3(bool enable = true)
		{
			Register::Ptr()->TIM3EN = enable;
		}
		
		static void EnableClockTimer4(bool enable = true)
		{
			Register::Ptr()->TIM4EN = enable;
		}
		
		static void EnableClockTimer5(bool enable = true)
		{
			Register::Ptr()->TIM5EN = enable;
		}
		
		static void EnableClockTimer6(bool enable = true)
		{
			Register::Ptr()->TIM6EN = enable;
		}
		
		static void EnableClockTimer7(bool enable = true)
		{
			Register::Ptr()->TIM7EN = enable;
		}
		
		static void EnableClockTimer12(bool enable = true)
		{
			Register::Ptr()->TIM12EN = enable;
		}
		
		static void EnableClockTimer13(bool enable = true)
		{
			Register::Ptr()->TIM13EN = enable;
		}
		
		static void EnableClockTimer14(bool enable = true)
		{
			Register::Ptr()->TIM14EN = enable;
		}
		
		static void EnableClockWindowWatchdog(bool enable = true)
		{
			Register::Ptr()->WWDGEN = enable;
		}
		
		static void EnableClockSpi2(bool enable = true)
		{
			Register::Ptr()->SPI2EN = enable;
		}
		
		static void EnableClockSpi3(bool enable = true)
		{
			Register::Ptr()->SPI3EN = enable;
		}
		
		static void EnableClockUsart2(bool enable = true)
		{
			Register::Ptr()->USART2EN = enable;
		}
		
		static void EnableClockUsart3(bool enable = true)
		{
			Register::Ptr()->USART3EN = enable;
		}
		
		static void EnableClockUart4(bool enable = true)
		{
			Register::Ptr()->UART4EN = enable;
		}
		
		static void EnableClockUart5(bool enable = true)
		{
			Register::Ptr()->UART5EN = enable;
		}
		
		static void EnableClockI2C1(bool enable = true)
		{
			Register::Ptr()->I2C1EN = enable;
		}
		
		static void EnableClockI2C2(bool enable = true)
		{
			Register::Ptr()->I2C2EN = enable;
		}
		
		static void EnableClockUsb(bool enable = true)
		{
			Register::Ptr()->USBEN = enable;
		}
		
		static void EnableClockCan(bool enable = true)
		{
			Register::Ptr()->CANEN = enable;
		}
		
		static void EnableClockBackupInterface(bool enable = true)
		{
			Register::Ptr()->BKPEN = enable;
		}
		
		static void EnableClockPowerInterface(bool enable = true)
		{
			Register::Ptr()->PWREN = enable;
		}
		
		static void EnableClockDac(bool enable = true)
		{
			Register::Ptr()->DACEN = enable;
		}
	};
	
	template
		<
			unsigned int IdObj
		>
	class ResetAndClockControl :
		public IdObjectBase<IdObj>,
		public ClockControlRegister<BaseAddress>,
		public ClockConfigurationRegister<BaseAddress>,
		public AHBPeripheralClockEnableRegister<BaseAddress>,
		public APB2PeripheralClockEnableRegister<BaseAddress>,
		public APB1PeripheralClockEnableRegister<BaseAddress>
	{
	};
}

typedef ResetAndClockControlImplementation::ResetAndClockControl<1> ResetAndClockControl;

}
}

#endif