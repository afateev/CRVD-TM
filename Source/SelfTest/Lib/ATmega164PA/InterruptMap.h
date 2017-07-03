///////////////////////////////////////////////////////////////////////////////
//	Класс управления вектором прерываний
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_ATMEGA164PA_H
#define INTERRUPTMAP_ATMEGA164PA_H

#include "..\Platform.h"
#include "..\CallbackWrapper.h"
#include "..\Usart.h"

namespace Lib
{
namespace ATmega164PA
{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_ATMEGA164PA
	class InterruptMap
	{
	public:
		// тип обработчика
		typedef CallbackWrapper<> CallbackType;
		
		// обработчики
		//static CallbackType HandlerINT0;
		//static CallbackType HandlerINT1;
		//static CallbackType HandlerINT2;
		//static CallbackType HandlerPCINT0;
		//static CallbackType HandlerPCINT1;
		//static CallbackType HandlerPCINT2;
		//static CallbackType HandlerPCINT3;
		//static CallbackType HandlerWDT;
		static CallbackType HandlerTIMER2_COMPA;
		static CallbackType HandlerTIMER2_COMPB;
		static CallbackType HandlerTIMER2_OVF;
		//static CallbackType HandlerTIMER1_CAPT;
		static CallbackType HandlerTIMER1_COMPA;
		static CallbackType HandlerTIMER1_COMPB;
		static CallbackType HandlerTIMER1_OVF;
		static CallbackType HandlerTIMER0_COMPA;
		static CallbackType HandlerTIMER0_COMPB;
		static CallbackType HandlerTIMER0_OVF;
		//static CallbackType HandlerSPI_STC;
		//static CallbackType HandlerUSART0_RX;
		//static CallbackType HandlerUSART0_UDRE;
		//static CallbackType HandlerUSART0_TX;
		//static CallbackType HandlerANALOG_COMP;
		//static CallbackType HandlerADC;
		//static CallbackType HandlerEE_READY;
		//static CallbackType HandlerTWI;
		//static CallbackType HandlerSPM_READY;
		static CallbackType HandlerUSART1_RX;
		//static CallbackType HandlerUSART1_UDRE;
		//static CallbackType HandlerUSART1_TX;
	public:
#ifdef __ICCAVR__
		#pragma vector = 0x04
		__interrupt static void INT0()
		{
			//HandlerINT0.Call();
		}
		
		#pragma vector = 0x08
		__interrupt static void INT1()
		{
			//HandlerINT1.Call();
		}
		
		#pragma vector = 0x0C
		__interrupt static void INT2()
		{
			//HandlerINT2.Call();
		}
		
		#pragma vector = 0x10
		__interrupt static void PCINT0()
		{
			//HandlerPCINT0.Call();
		}
		
		#pragma vector = 0x14
		__interrupt static void PCINT1()
		{
			//HandlerPCINT1.Call();
		}
		
		#pragma vector = 0x18
		__interrupt static void PCINT2()
		{
			//HandlerPCINT2.Call();
		}
		
		#pragma vector = 0x1C
		__interrupt static void PCINT3()
		{
			//HandlerPCINT3.Call();
		}
		
		#pragma vector = 0x20
		__interrupt static void WDT()
		{
			//HandlerWDT.Call();
		}
		
		#pragma vector = 0x24
		__interrupt static void TIMER2_COMPA()
		{
			HandlerTIMER2_COMPA.Call();
		}
		
		#pragma vector = 0x28
		__interrupt static void TIMER2_COMPB()
		{
			HandlerTIMER2_COMPB.Call();
		}
		
		#pragma vector = 0x2C
		__interrupt static void TIMER2_OVF()
		{
			HandlerTIMER2_OVF.Call();
		}
		
		#pragma vector = 0x30
		__interrupt static void TIMER1_CAPT()
		{
			//HandlerTIMER1_CAPT.Call();
		}
		
		#pragma vector = 0x34
		__interrupt static void TIMER1_COMPA()
		{
			HandlerTIMER1_COMPA.Call();
		}
		
		#pragma vector = 0x38
		__interrupt static void TIMER1_COMPB()
		{
			HandlerTIMER1_COMPB.Call();
		}
		
		#pragma vector = 0x3C
		__interrupt static void TIMER1_OVF()
		{
			HandlerTIMER1_OVF.Call();
		}
		
		#pragma vector = 0x40
		__interrupt static void TIMER0_COMPA()
		{
			HandlerTIMER0_COMPA.Call();
		}
		
		#pragma vector = 0x44
		__interrupt static void TIMER0_COMPB()
		{
			HandlerTIMER0_COMPB.Call();
		}
		
		#pragma vector = 0x48
		__interrupt static void TIMER0_OVF()
		{
			HandlerTIMER0_OVF.Call();
		}
		
		#pragma vector = 0x4C
		__interrupt static void SPI_STC()
		{
			//HandlerSPI_STC.Call();
		}
		
		#pragma vector = 0x50
		__interrupt static void USART0_RX()
		{
			//HandlerUSART0_RX.Call();
		}
		
		#pragma vector = 0x54
		__interrupt static void USART0_UDRE()
		{
			Usart::Usart0::OnDataRegisterEmptyInterrupt();
			//HandlerUSART0_UDRE.Call();
		}
		
		#pragma vector = 0x58
		__interrupt static void USART0_TX()
		{
			Usart::Usart0::OnTxCompleteInterrupt();
			//HandlerUSART0_TX.Call();
		}
		
		#pragma vector = 0x5C
		__interrupt static void ANALOG_COMP()
		{
			//HandlerANALOG_COMP.Call();
		}
		
		#pragma vector = 0x60
		__interrupt static void ADC()
		{
			//HandlerADC.Call();
		}
		
		#pragma vector = 0x64
		__interrupt static void EE_READY()
		{
			//HandlerEE_READY.Call();
		}
		
		#pragma vector = 0x68
		__interrupt static void TWI()
		{
			//HandlerTWI.Call();
		}
		
		#pragma vector = 0x6C
		__interrupt static void SPM_READY()
		{
			//HandlerSPM_READY.Call();
		}
		
		#pragma vector = 0x70
		__interrupt static void USART1_RX()
		{
			HandlerUSART1_RX.Call();
		}
		
		#pragma vector = 0x74
		__interrupt static void USART1_UDRE()
		{
			Usart::Usart1::OnDataRegisterEmptyInterrupt();
			//HandlerUSART1_UDRE.Call();
		}
		
		#pragma vector = 0x78
		__interrupt static void USART1_TX()
		{
			Usart::Usart1::OnTxCompleteInterrupt();
			//HandlerUSART1_TX.Call();
		}
		
		static inline void GlobalInterruptEnable(void)
		{
			asm("sei");
		}
		
		static inline void GlobalInterruptDisable(void)
		{
			asm("cli");
		}
#endif //__ICCAVR__
	};
	
	//InterruptMap::CallbackType InterruptMap::HandlerINT0;
	//InterruptMap::CallbackType InterruptMap::HandlerINT1;
	//InterruptMap::CallbackType InterruptMap::HandlerINT2;
	//InterruptMap::CallbackType InterruptMap::HandlerPCINT0;
	//InterruptMap::CallbackType InterruptMap::HandlerPCINT1;
	//InterruptMap::CallbackType InterruptMap::HandlerPCINT2;
	//InterruptMap::CallbackType InterruptMap::HandlerPCINT3;
	//InterruptMap::CallbackType InterruptMap::HandlerWDT;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER2_COMPA;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER2_COMPB;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER2_OVF;
	//InterruptMap::CallbackType InterruptMap::HandlerTIMER1_CAPT;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER1_COMPA;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER1_COMPB;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER1_OVF;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER0_COMPA;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER0_COMPB;
	InterruptMap::CallbackType InterruptMap::HandlerTIMER0_OVF;
	//InterruptMap::CallbackType InterruptMap::HandlerSPI_STC;
	//InterruptMap::CallbackType InterruptMap::HandlerUSART0_RX;
	//InterruptMap::CallbackType InterruptMap::HandlerUSART0_UDRE;
	//InterruptMap::CallbackType InterruptMap::HandlerUSART0_TX;
	//InterruptMap::CallbackType InterruptMap::HandlerANALOG_COMP;
	//InterruptMap::CallbackType InterruptMap::HandlerADC;
	//InterruptMap::CallbackType InterruptMap::HandlerEE_READY;
	//InterruptMap::CallbackType InterruptMap::HandlerTWI;
	//InterruptMap::CallbackType InterruptMap::HandlerSPM_READY;
	InterruptMap::CallbackType InterruptMap::HandlerUSART1_RX;
	//InterruptMap::CallbackType InterruptMap::HandlerUSART1_UDRE;
	//InterruptMap::CallbackType InterruptMap::HandlerUSART1_TX;
#else
	class InterruptMap
	{
	};
#endif
}
}

#endif
