#ifndef RCC_STM32_F4_H
#define RCC_STM32_F4_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace F4
{
namespace ResetAndClockControlImplementation
{
	static const unsigned int BaseAddress = 0x40023800;
	
	// RCC clock control register (RCC_CR)
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
			Core::RegisterValueType PLLI2SON								: 1;
			Core::RegisterValueType PLLI2SRDY								: 1;
			Core::RegisterValueType PLLSAION								: 1;
			Core::RegisterValueType PLLSAIRDY								: 1;
			Core::RegisterValueType											: 4;
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
		
		static void PLLI2SEnable(bool enable = true)
		{
			Register::Ptr()->PLLI2SON = enable;
		}
		
		static bool PLLI2SReady()
		{
			return Register::Ptr()->PLLI2SRDY;
		}
		
		static void PLLSAIEnable(bool enable = true)
		{
			Register::Ptr()->PLLSAION = enable;
		}
		
		static bool PLLSAIReady()
		{
			return Register::Ptr()->PLLSAIRDY;
		}
	};
	
	// RCC PLL configuration register (RCC_PLLCFGR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PllConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x04;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum PLLSource
		{
			PLLSourceHSI		= 0,
			PLLSourceHSE		= 1,
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType PLLM									: 6;
			Core::RegisterValueType PLLN									: 9;
			Core::RegisterValueType 										: 1;
			Core::RegisterValueType PLLP									: 2;
			Core::RegisterValueType 										: 4;
			Core::RegisterValueType PLLSRC									: 1;
			Core::RegisterValueType 										: 1;
			Core::RegisterValueType PLLQ									: 4;
			Core::RegisterValueType											: 4;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetPLLM(unsigned char value)
		{
			Register::Ptr()->PLLM = value;
		}
		
		static void SetPLLN(unsigned char value)
		{
			Register::Ptr()->PLLN = value;
		}
		
		static void SetPLLP(unsigned char value)
		{
			Register::Ptr()->PLLP = value / 2 - 1;
		}
		
		static void SetPLLSource(PLLSource source)
		{
			Register::Ptr()->PLLSRC = source;
		}
		
		static void SetPLLQ(unsigned char value)
		{
			Register::Ptr()->PLLQ = value;
		}
	};
	
	// RCC clock configuration register (RCC_CFGR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class ClockConfigurationRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x08;
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
			APB1PrescalerAHB			= 0,
			APB1PrescalerAHBDiv2		= 4,
			APB1PrescalerAHBDiv4		= 5,
			APB1PrescalerAHBDiv8		= 6,
			APB1PrescalerAHBDiv16		= 7,
		};
		
		enum APB2Prescaler
		{
			APB2PrescalerAHB			= 0,
			APB2PrescalerAHBDiv2		= 4,
			APB2PrescalerAHBDiv4		= 5,
			APB2PrescalerAHBDiv8		= 6,
			APB2PrescalerAHBDiv16		= 7,
		};
		
		enum McoPrescaler
		{
			McoPrescalerDiv1		= 0,
			McoPrescalerDiv2		= 4,
			McoPrescalerDiv3		= 5,
			McoPrescalerDiv4		= 6,
			McoPrescalerDiv5		= 7,
		};
		
		enum ClockOutput1
		{
			ClockOutput1HSI		= 0,
			ClockOutput1LSE		= 1,
			ClockOutput1HSE		= 2,
			ClockOutput1Pll		= 3
		};
		
		enum ClockOutput2
		{
			ClockOutput2SysClk	= 0,
			ClockOutput2PLLI2S	= 1,
			ClockOutput2HSE		= 2,
			ClockOutput2Pll		= 3
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType SW										: 2;
			Core::RegisterValueType SWS										: 2;
			Core::RegisterValueType HPRE									: 4;
			Core::RegisterValueType 										: 2;
			Core::RegisterValueType PPRE1									: 3;
			Core::RegisterValueType PPRE2									: 3;
			Core::RegisterValueType RTCPRE									: 5;
			Core::RegisterValueType MCO1									: 2;
			Core::RegisterValueType I2SSRC									: 1;
			Core::RegisterValueType MCO1PRE									: 3;
			Core::RegisterValueType MCO2PRE									: 3;
			Core::RegisterValueType MCO2									: 2;
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
		
		static void SetClockOutput1Prescaler(McoPrescaler val)
		{
			Register::Ptr()->MCO1PRE = val;
		}
		
		static void SetClockOutput1(ClockOutput1 clockOutput)
		{
			Register::Ptr()->MCO1 = clockOutput;
		}
		
		static void SetClockOutput2Prescaler(McoPrescaler val)
		{
			Register::Ptr()->MCO2PRE = val;
		}
		
		static void SetClockOutput2(ClockOutput2 clockOutput)
		{
			Register::Ptr()->MCO2 = clockOutput;
		}
	};
	
	// RCC AHB1 peripheral clock enable register (RCC_AHB1ENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AHB1PeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x30;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType GPIOAEN			: 1;
			Core::RegisterValueType GPIOBEN			: 1;
			Core::RegisterValueType GPIOCEN			: 1;
			Core::RegisterValueType GPIODEN			: 1;
			Core::RegisterValueType GPIOEEN			: 1;
			Core::RegisterValueType GPIOFEN			: 1;
			Core::RegisterValueType GPIOGEN			: 1;
			Core::RegisterValueType GPIOHEN			: 1;
			Core::RegisterValueType GPIOIEN			: 1;
			Core::RegisterValueType GPIOJEN			: 1;
			Core::RegisterValueType GPIOKEN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType CRCEN			: 1;
			Core::RegisterValueType 				: 6;
			Core::RegisterValueType BKPSRAMEN		: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType CCMDATARAMEN	: 1;
			Core::RegisterValueType DMA1EN			: 1;
			Core::RegisterValueType DMA2EN			: 1;
			Core::RegisterValueType 				: 2;
			Core::RegisterValueType ETHMACEN		: 1;
			Core::RegisterValueType ETHMACTXEN		: 1;
			Core::RegisterValueType ETHMACRXEN		: 1;
			Core::RegisterValueType ETHMACPTPEN		: 1;
			Core::RegisterValueType OTGHSEN			: 1;
			Core::RegisterValueType OTGHSULPIEN		: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockPortA(bool enable = true)
		{
			Register::Ptr()->GPIOAEN = enable;
		}
		
		static void EnableClockPortB(bool enable = true)
		{
			Register::Ptr()->GPIOBEN = enable;
		}
		
		static void EnableClockPortC(bool enable = true)
		{
			Register::Ptr()->GPIOCEN = enable;
		}
		
		static void EnableClockPortD(bool enable = true)
		{
			Register::Ptr()->GPIODEN = enable;
		}
		
		static void EnableClockPortE(bool enable = true)
		{
			Register::Ptr()->GPIOEEN = enable;
		}
		
		static void EnableClockPortF(bool enable = true)
		{
			Register::Ptr()->GPIOFEN = enable;
		}
		
		static void EnableClockPortG(bool enable = true)
		{
			Register::Ptr()->GPIOGEN = enable;
		}
		
		static void EnableClockPortH(bool enable = true)
		{
			Register::Ptr()->GPIOHEN = enable;
		}
		
		static void EnableClockPortI(bool enable = true)
		{
			Register::Ptr()->GPIOIEN = enable;
		}
		
		static void EnableClockPortJ(bool enable = true)
		{
			Register::Ptr()->GPIOJEN = enable;
		}
		
		static void EnableClockPortK(bool enable = true)
		{
			Register::Ptr()->GPIOKEN = enable;
		}
		
		static void EnableClockDma1(bool enable = true)
		{
			Register::Ptr()->DMA1EN = enable;
		}
		
		static void EnableClockDma2(bool enable = true)
		{
			Register::Ptr()->DMA2EN = enable;
		}
	};
	
	// RCC AHB2 peripheral clock enable register (RCC_AHB2ENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AHB2PeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x34;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType DCMIEN			: 1;
			Core::RegisterValueType 				: 3;
			Core::RegisterValueType CRYPEN			: 1;
			Core::RegisterValueType HASHEN			: 1;
			Core::RegisterValueType RNGEN			: 1;
			Core::RegisterValueType OTGFS			: 1;
			Core::RegisterValueType 				: 24;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockOTGFS(bool enable = true)
		{
			Register::Ptr()->OTGFS = enable;
		}
	};
	
	// RCC AHB3 peripheral clock enable register (RCC_AHB3ENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class AHB3PeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x38;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType FSMCEN			: 1;
			Core::RegisterValueType 				: 31;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockFSMCEN(bool enable = true)
		{
			Register::Ptr()->FSMCEN = enable;
		}
	};
	
	// RCC APB1 peripheral clock enable register (RCC_APB1ENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class APB1PeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x40;
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
			Core::RegisterValueType USART4EN		: 1;
			Core::RegisterValueType USART5EN		: 1;
			Core::RegisterValueType I2C1EN			: 1;
			Core::RegisterValueType I2C2EN			: 1;
			Core::RegisterValueType I2C3EN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType CAN1EN			: 1;
			Core::RegisterValueType CAN2EN			: 1;
			Core::RegisterValueType 				: 1;
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
		
		static void EnableClockCan1(bool enable = true)
		{
			Register::Ptr()->CAN1EN = enable;
		}
		
		static void EnableClockCan2(bool enable = true)
		{
			Register::Ptr()->CAN2EN = enable;
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
	
	// RCC APB2 peripheral clock enable register (RCC_APB2ENR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class APB2PeripheralClockEnableRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x44;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType TIM1EN			: 1;
			Core::RegisterValueType TIM8EN			: 1;
			Core::RegisterValueType 				: 2;
			Core::RegisterValueType USART1EN		: 1;
			Core::RegisterValueType USART6EN		: 1;
			Core::RegisterValueType 				: 2;
			Core::RegisterValueType ADC1EN			: 1;
			Core::RegisterValueType ADC2EN			: 1;
			Core::RegisterValueType ADC3EN			: 1;
			Core::RegisterValueType SDIOEN			: 1;
			Core::RegisterValueType SPI1EN			: 1;
			Core::RegisterValueType SPI4EN			: 1;
			Core::RegisterValueType SYSCFGEN		: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType TIM9EN			: 1;
			Core::RegisterValueType TIM10EN			: 1;
			Core::RegisterValueType TIM11EN			: 1;
			Core::RegisterValueType 				: 1;
			Core::RegisterValueType SPI5EN			: 1;
			Core::RegisterValueType SPI6EN			: 1;
			Core::RegisterValueType 				: 10;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockTimer1(bool enable = true)
		{
			Register::Ptr()->TIM1EN = enable;
		}
		
		static void EnableClockTimer8(bool enable = true)
		{
			Register::Ptr()->TIM8EN = enable;
		}
		
		static void EnableClockUsart1(bool enable = true)
		{
			Register::Ptr()->USART1EN = enable;
		}
		
		static void EnableClockUsart6(bool enable = true)
		{
			Register::Ptr()->USART6EN = enable;
		}
		
		static void EnableClockAdc1(bool enable = true)
		{
			Register::Ptr()->ADC1EN = enable;
		}
		
		static void EnableClockAdc2(bool enable = true)
		{
			Register::Ptr()->ADC2EN = enable;
		}
		
		static void EnableClockAdc3(bool enable = true)
		{
			Register::Ptr()->ADC3EN = enable;
		}
		
		static void EnableClockSdio(bool enable = true)
		{
			Register::Ptr()->SDIOEN = enable;
		}
		
		static void EnableClockSpi1(bool enable = true)
		{
			Register::Ptr()->SPI1EN = enable;
		}
		
		static void EnableClockSpi4(bool enable = true)
		{
			Register::Ptr()->SPI4EN = enable;
		}
		
		static void EnableClockSYSCFG(bool enable = true)
		{
			Register::Ptr()->SYSCFGEN = enable;
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
		
		static void EnableClockSpi5(bool enable = true)
		{
			Register::Ptr()->SPI5EN = enable;
		}
		
		static void EnableClockSpi6(bool enable = true)
		{
			Register::Ptr()->SPI6EN = enable;
		}
	};
	
	// RCC Backup domain control register (RCC_BDCR)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class BackupDomainControlRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x70;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum RtcSource
		{
			RtcSourceNoCLock		= 0,
			RtcSourceLSE			= 1,
			RtcSourceLSI			= 2,
			RtcSourceHSE			= 3,
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType LSEON									: 1;
			Core::RegisterValueType LSERDY									: 1;
			Core::RegisterValueType LSEBYP									: 1;
			Core::RegisterValueType 										: 5;
			Core::RegisterValueType RTCSEL									: 2;
			Core::RegisterValueType 										: 5;
			Core::RegisterValueType RTCEN									: 1;
			Core::RegisterValueType BDRST									: 1;
			Core::RegisterValueType 										: 15;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void LSEEnable(bool enable = true)
		{
			Register::Ptr()->LSEON = enable;
		}
		
		static bool LSEReady()
		{
			return Register::Ptr()->LSERDY;
		}
		
		static void SetRtcSource(RtcSource source)
		{
			Register::Ptr()->RTCSEL = source;
		}
		
		static void RtcEnable(bool enable = true)
		{
			Register::Ptr()->RTCEN = enable;
		}
		
		static bool IsRtcEnabled()
		{
			return Register::Ptr()->RTCEN;
		}
	};
	
	template
		<
			unsigned int IdObj
		>
	class ResetAndClockControl :
		public IdObjectBase<IdObj>,
		public ClockControlRegister<BaseAddress>,
		public PllConfigurationRegister<BaseAddress>,
		public ClockConfigurationRegister<BaseAddress>,
		public AHB1PeripheralClockEnableRegister<BaseAddress>,
		public AHB2PeripheralClockEnableRegister<BaseAddress>,
		public AHB3PeripheralClockEnableRegister<BaseAddress>,
		public APB1PeripheralClockEnableRegister<BaseAddress>,
		public APB2PeripheralClockEnableRegister<BaseAddress>,
		public BackupDomainControlRegister<BaseAddress>
	{
	};
}

typedef ResetAndClockControlImplementation::ResetAndClockControl<1> ResetAndClockControl;

}
}
}

#endif