///////////////////////////////////////////////////////////////////////////////
//	Класс управления вектором прерываний
//	MSP430F2618
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_MSP430F2618_H
#define INTERRUPTMAP_MSP430F2618_H

#include "../Platform.h"
#include "Uart.h"
#include "I2C.h"
#include "..\msp430\InterruptsState.h"

namespace Rblib
{
namespace Msp430f2618
{
	class InterruptMap : public Msp430::InterrupsState<RBLIB_PALTFORM_AUTODETECT_ID>
	{
	public:
		// Вектора прерываний не должны перекрываться,
		// поэтому объявляем их только если выбрана данная платформа
		#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F2618
		// DAC12
		RBLIB_ISR(14 * 2u)
		static void IsrDac()
		{
			
		}

		// DMA
		RBLIB_ISR(15 * 2u)
		static void IsrDma()
		{
			
		}

		// USCI A1/B1 Transmit
		RBLIB_ISR(16 * 2u)
		static void IsrUsciAB1Tx()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			// USCI_A
			if (UartHardware::_B::IsUartMode())
			{
				// UART
				UartHardware::_B::TxHandler();
			}
			else
			{
				// SPI
			}

			// USCI_B
			if (I2CHardware::_B::IsI2CMode())
			{
				// I2C
				I2CHardware::_B::InterruptHandler();
			}
			else
			{
				// SPI
			}
		}

		// USCI A1/B1 Receive
		RBLIB_ISR(17 * 2u)
		static void IsrUsciAB1Rx()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			// USCI_A
			if (UartHardware::_B::IsUartMode())
			{
				// UART
				UartHardware::_B::RxHandler();
			}
			else
			{
				// SPI
			}

			// USCI_B
			if (I2CHardware::_B::IsI2CMode())
			{
				// I2C
				I2CHardware::_B::InterruptHandler();
			}
			else
			{
				// SPI
			}
		}

		// Port 1
		RBLIB_ISR(18 * 2u)
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

		// Port 2
		RBLIB_ISR(19 * 2u)
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

		// ADC
		RBLIB_ISR(21 * 2u)
		static void IsrAdc()
		{
			
		}

		// USCI A0/B0 Transmit
		RBLIB_ISR(22 * 2u)
		static void IsrUsciAB0Tx()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			// USCI_A
			if (UartHardware::_A::IsUartMode())
			{
				// UART
				UartHardware::_A::TxHandler();
			}
			else
			{
				// SPI
			}

			// USCI_B
			if (I2CHardware::_A::IsI2CMode())
			{
				// I2C
				I2CHardware::_A::InterruptHandler();
			}
			else
			{
				// SPI
			}
		}

		// USCI A0/B0 Receive
		RBLIB_ISR(23 * 2u)
		static void IsrUsciAB0Rx()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			// USCI_A
			if (UartHardware::_A::IsUartMode())
			{
				// UART
				UartHardware::_A::RxHandler();
			}
			else
			{
				// SPI
			}

			// USCI_B
			if (I2CHardware::_A::IsI2CMode())
			{
				// I2C
				I2CHardware::_A::InterruptHandler();
			}
			else
			{
				// SPI
			}
		}

		// Timer A CC1-2, TA
		RBLIB_ISR(24 * 2u)
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
		RBLIB_ISR(25 * 2u)
		static void IsrTimerA0()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_A::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// Watchdog Timer
		RBLIB_ISR(26 * 2u)
		static void IsrWdt()
		{
			
		}

		// Comparator A
		RBLIB_ISR(27 * 2u)
		static void IsrComparatorA()
		{
			
		}

		// Timer B CC1-6, TB
		RBLIB_ISR(28 * 2u)
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
		RBLIB_ISR(29 * 2u)
		static void IsrTimerB0()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_B::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// Non-maskable
		RBLIB_ISR(30 * 2u)
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