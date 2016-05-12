#ifndef INTERRUPTMAP_STM32_F4_H
#define INTERRUPTMAP_STM32_F4_H

#include "../../Utils/CallbackWrapper.h"
#include "../../Utils/CallbackListWrapper.h"
#include "../../Timer.h"
#include "../../Usart.h"

namespace Rblib
{
namespace Stm32
{
namespace F4
{

#ifdef STM32F4

#ifdef __ICCARM__
	#pragma segment="CSTACK"
	extern "C" void __iar_program_start( void );
	
	template
		<
			int platformId
		>
	class InterruptMap
	{
	public:
		static const int MaxHandlersCount = 8;
		typedef CallbackListWrapper<MaxHandlersCount, int> UndefinedInterruptHandlerType;
		enum FaultCode
		{
			FaultCodeHard,
			FaultCodeMemoryManagement,
			FaultCodeBus,
			FaultCodeUsage,
		};
		typedef CallbackWrapper<FaultCode> FaultHandlerType;
		
	public:
		static UndefinedInterruptHandlerType UndefinedInterruptHandler;
		static FaultHandlerType FaultHandler;
	public:
		static void NMI()
		{
			while (1) {}
		}
		
		static void HardFault()
		{
			FaultHandler(FaultCodeHard);
			while (1) {}
		}
		
		static void MemoryManagementFault()
		{
			FaultHandler(FaultCodeMemoryManagement);
			while (1) {}
		}
		
		static void BusFault()
		{
			FaultHandler(FaultCodeBus);
			while (1) {}
		}
		
		static void UsageFault()
		{
			FaultHandler(FaultCodeUsage);
			while (1) {}
		}
		
		static void SVCall()
		{
		}
		
		static void PendSV()
		{
		}
		
		static void SystemTick()
		{
			
		}
		
		static void WWDG()
		{
		}
		
		static void PVD()
		{
		}
		
		static void TAMPER_STAMP()
		{
		}
		
		static void RTC_WKUP()
		{
		}
		
		static void FLASH()
		{
		}
		
		static void RCC()
		{
		}
		
		static void EXTI0()
		{
		}
		
		static void EXTI1()
		{
		}
		
		static void EXTI2()
		{
		}
		
		static void EXTI3()
		{
		}
		
		static void EXTI4()
		{
		}
		
		static void DMA1_Stream0()
		{
		}
		
		static void DMA1_Stream1()
		{
		}
		
		static void DMA1_Stream2()
		{
		}
		
		static void DMA1_Stream3()
		{
		}
		
		static void DMA1_Stream4()
		{
		}
		
		static void DMA1_Stream5()
		{
		}
		
		static void DMA1_Stream6()
		{
		}
		
		static void ADC()
		{
		}
		
		static void CAN1_TX()
		{
		}
		
		static void CAN1_RX0()
		{
		}
		
		static void CAN1_RX1()
		{
		}
		
		static void CAN1_SCE()
		{
		}
		
		static void EXTI9_5()
		{
		}
		
		static void TIM1_BRK_TIM9()
		{
			Timer::Timer1::InterruptHandler_BRK();
		}
		
		static void TIM1_UP_TIM10()
		{
			Timer::Timer1::InterruptHandler_UP();
		}
		
		static void TIM1_TRG_COM_TIM11()
		{
			Timer::Timer1::InterruptHandler_TRG_COM();
		}
		
		static void TIM1_CC()
		{
			Timer::Timer1::InterruptHandler_CC();
		}
		
		static void TIM2()
		{
			Timer::Timer2::InterruptHandler();
		}
		
		static void TIM3()
		{
			Timer::Timer3::InterruptHandler();
		}
		
		static void TIM4()
		{
			Timer::Timer4::InterruptHandler();
		}
		
		static void I2C1_EV()
		{
		}
		
		static void I2C1_ER()
		{
		}
		
		static void I2C2_EV()
		{
		}
		
		static void I2C2_ER()
		{
		}
		
		static void SPI1()
		{
		}
		
		static void SPI2()
		{
		}
		
		static void USART1()
		{
			Usart::Usart1::InterruptHandler();
		}
		
		static void USART2()
		{
			Usart::Usart2::InterruptHandler();
		}
		
		static void USART3()
		{
			Usart::Usart3::InterruptHandler();
		}
		
		static void EXTI15_10()
		{
		}
		
		static void RTCAlarm()
		{
		}
		
		static void OTG_FS_WKUP()
		{
		}
		
		static void TIM8_BRK_TIM12()
		{
			Timer::Timer1::InterruptHandler_BRK();
		}
		
		static void TIM8_UP_TIM13()
		{
			Timer::Timer1::InterruptHandler_UP();
		}
		
		static void TIM8_TRG_COM_TIM14()
		{
			Timer::Timer1::InterruptHandler_TRG_COM();
		}
		
		static void TIM8_CC()
		{
			Timer::Timer1::InterruptHandler_CC();
		}
		
		static void DMA1_Stream7()
		{
		}
		
		static void FSMC()
		{
		}
		
		static void SDIO()
		{
		}
		
		static void TIM5()
		{
			Timer::Timer5::InterruptHandler();
		}
		
		static void SPI3()
		{
		}
		
		static void UART4()
		{
			Usart::Uart4::InterruptHandler();
		}
		
		static void UART5()
		{
			Usart::Uart5::InterruptHandler();
		}
		
		static void TIM6_DAC()
		{
		}
		
		static void TIM7()
		{
		}
		
		static void DMA2_Stream0()
		{
		}
		
		static void DMA2_Stream1()
		{
		}
		
		static void DMA2_Stream2()
		{
		}
		
		static void DMA2_Stream3()
		{
		}
		
		static void DMA2_Stream4()
		{
		}
		
		static void ETH()
		{
		}
		
		static void ETH_WKUP()
		{
		}
		
		static void CAN2_TX()
		{
		}
		
		static void CAN2_RX0()
		{
		}
		
		static void CAN2_RX1()
		{
		}
		
		static void CAN2_SCE()
		{
		}
		
		static void OTG_FS()
		{
		}
		
		static void DMA2_Stream5()
		{
		}
		
		static void DMA2_Stream6()
		{
		}
		
		static void DMA2_Stream7()
		{
		}
		
		static void USART6()
		{
		}
		
		static void I2C3_EV()
		{
		}
		
		static void I2C3_ER()
		{
		}
		
		static void OTG_HS_EP1_OUT()
		{
		}
		
		static void OTG_HS_EP1_IN()
		{
		}
		
		static void OTG_HS_WKUP()
		{
		}
		
		static void OTG_HS()
		{
		}
		
		static void DCMI()
		{
		}
		
		static void CRYP()
		{
		}
		
		static void HASH_RNG()
		{
		}
		
		static void FPU()
		{
		}
		
		static void UART7()
		{
		}
		
		static void UART8()
		{
		}
		
		static void SPI4()
		{
		}
		
		static void SPI5()
		{
		}
		
		static void SPI6()
		{
		}
		
		static void SAI1()
		{
		}
		
		static void LCD_TFT()
		{
		}
		
		static void LCD_TFT_ERROR()
		{
		}
		
		static void DMAD2()
		{
		}

		// –азрешить прерывани€
		static inline void GlobalInterruptEnable(void)
		{
			RBLIB_GLOBAL_INTERRUPT_ENABLE;
		}

		// «апретить прерывани€
		static inline void GlobalInterruptDisable(void)
		{
			RBLIB_GLOBAL_INTERRUPT_DISABLE;
		}
	};
	
	template < int platformId >
		InterruptMap<platformId>::UndefinedInterruptHandlerType InterruptMap<platformId>::UndefinedInterruptHandler;
	
	template < int platformId >
		InterruptMap<platformId>::FaultHandlerType InterruptMap<platformId>::FaultHandler;
	
	typedef void( *intfunc )( void );
	typedef union { intfunc __fun; void * __ptr; } intvec_elem;
	
	// константный вектор прерываний
	// —оздаетс€ во врем€ линковки, хранитс€ в Flash
	#pragma location = ".intvec"
	extern "C" const intvec_elem __vector_table[] =
		{
			{ .__ptr = __sfe( "CSTACK" ) },
			__iar_program_start,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::NMI,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::HardFault,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::MemoryManagementFault,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::BusFault,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UsageFault,
			0,
			0,
			0,
			0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SVCall,
			0,
			0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::PendSV,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SystemTick,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::WWDG,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::PVD,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TAMPER_STAMP,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::RTC_WKUP,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::FLASH,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::RCC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EXTI0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EXTI1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EXTI2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EXTI3,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EXTI4,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream3,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream4,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream5,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream6,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::ADC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN1_TX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN1_RX0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN1_RX1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN1_SCE,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EXTI9_5,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM1_BRK_TIM9,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM1_UP_TIM10,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM1_TRG_COM_TIM11,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM1_CC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM3,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM4,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C1_EV,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C1_ER,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C2_EV,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C2_ER,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SPI1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SPI2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART3,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EXTI15_10,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::RTCAlarm,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::OTG_FS_WKUP,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM8_BRK_TIM12,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM8_UP_TIM13,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM8_TRG_COM_TIM14,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM8_CC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA1_Stream7,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::FSMC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SDIO,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM5,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SPI3,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART4,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART5,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM6_DAC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIM7,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream3,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream4,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::ETH,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::ETH_WKUP,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN2_TX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN2_RX0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN2_RX1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CAN2_SCE,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::OTG_FS,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream5,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream6,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA2_Stream7,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART6,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C3_EV,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C3_ER,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::OTG_HS_EP1_OUT,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::OTG_HS_EP1_IN,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::OTG_HS_WKUP,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::OTG_HS,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DCMI,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CRYP,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::HASH_RNG,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::FPU,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART7,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART8,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SPI4,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SPI5,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SPI6,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::SAI1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::LCD_TFT,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::LCD_TFT_ERROR,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMAD2
		};
#endif
	
#else
	template
		<
			int platformId
		>
	class InterruptMap
	{
	};
#endif
}
}
}

#endif