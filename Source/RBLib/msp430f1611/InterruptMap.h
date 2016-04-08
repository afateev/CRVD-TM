///////////////////////////////////////////////////////////////////////////////
//	Класс управления вектором прерываний
//	MSP430F1611
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_MSP430F1611_H
#define INTERRUPTMAP_MSP430F1611_H

#include "../Platform.h"
#include "..\msp430\InterruptsState.h"
#include "..\Common.h"
#include "Uart.h"
#include "I2C.h"

namespace Rblib
{
namespace Msp430f1611
{
	class InterruptMap : public Msp430::InterrupsState<RBLIB_PALTFORM_AUTODETECT_ID>
	{
	public:
		// Вектора прерываний не должны перекрываться,
		// поэтому объявляем их только если выбрана данная платформа
		#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611
		// DAC/DMA
		RBLIB_ISR(0 * 2u)
		static void IsrDacDma()
		{
			
		}

		// Port 2
		RBLIB_ISR(1 * 2u)
		static void IsrPort2()
		{
			RESET_WAKE_UP;
			for (unsigned char i = 0; i < 8; i++)
			{
				if (Gpio::Port2::GetInterruptFlag(i))
				{
					Event e(EventGpioPinChanged, Gpio::Port2::Id, i, Gpio::Port2::Read(i) > 0 ? 1 : 0);
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
			}
			Gpio::Port2::SetInterruptFlags(0x00);
			CHECK_WAKE_UP;
		}

		// USART 1 Transmit
		RBLIB_ISR(2 * 2u)
		static void IsrUsart1Tx()
		{
			UartHardware::_B::TxHandler();
		}

		// USART 1 Receive
		RBLIB_ISR(3 * 2u)
		static void IsrUsart1Rx()
		{
			UartHardware::_B::RxHandler();
		}

		// Port 1
		RBLIB_ISR(4 * 2u)
		static void IsrPort1()
		{
			RESET_WAKE_UP;
			for (unsigned char i = 0; i < 8; i++)
			{
				if (Gpio::Port1::GetInterruptFlag(i))
				{
					Event e(EventGpioPinChanged, Gpio::Port1::Id, i, Gpio::Port1::Read(i) > 0 ? 1 : 0);
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
			}
			Gpio::Port1::SetInterruptFlags(0x00);
			CHECK_WAKE_UP;
		}

		// Timer A CC1-2, TA
		RBLIB_ISR(5 * 2u)
		static void IsrTimerA1()
		{
			RESET_WAKE_UP;
			unsigned short vector = Timer::_A::GetInterruptVector();
			if (vector == 0x0A)
			{
				Event e(EventTimerOverflow, Timer::_A::Id);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			else
			{
				unsigned char regNumber = vector >> 1;
				Event e(EventTimerCompare, Timer::_A::Id, regNumber);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			CHECK_WAKE_UP;
		}

		// Timer A CC0
		RBLIB_ISR(6 * 2u)
		static void IsrTimerA0()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_A::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// ADC
		RBLIB_ISR(7 * 2u)
		static void IsrAdc()
		{
			
		}

		// USART 0 Transmit
		RBLIB_ISR(8 * 2u)
		static void IsrUsart0Tx()
		{
			UartHardware::_A::TxHandler();
		}

		// USART 0 Receive
		RBLIB_ISR(9 * 2u)
		static void IsrUsart0Rx()
		{
			UartHardware::_A::RxHandler();
		}

		// Watchdog Timer
		RBLIB_ISR(10 * 2u)
		static void IsrWdt()
		{
			
		}

		// Comparator A
		RBLIB_ISR(11 * 2u)
		static void IsrComparatorA()
		{
			
		}

		// Timer B CC1-6, TB
		RBLIB_ISR(12 * 2u)
		static void IsrTimerB1()
		{
			RESET_WAKE_UP;
			unsigned short vector = Timer::_B::GetInterruptVector();
			if (vector == 0x0E)
			{
				Event e(EventTimerOverflow, Timer::_B::Id);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			else
			{
				unsigned char regNumber = vector >> 1;
				Event e(EventTimerCompare, Timer::_B::Id, regNumber);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			CHECK_WAKE_UP;
		}

		// Timer B CC0
		RBLIB_ISR(13 * 2u)
		static void IsrTimerB0()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_B::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// Non-maskable
		RBLIB_ISR(14 * 2u)
		static void IsrNmi()
		{
			
		}
		#endif
		// Разрешить прерывания
		static inline void GlobalInterruptEnable(void)
		{
			RBLIB_ASM("eint");
		}

		// Запретить прерывания
		static inline void GlobalInterruptDisable(void)
		{
			RBLIB_ASM("dint");
		}
	};

	

	//#pragma location = 0xFFE8
	//static const unsigned int _globalMainIntVec = (unsigned int)InterruptMap::Vector1;
}
}

#endif