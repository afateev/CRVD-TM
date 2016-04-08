///////////////////////////////////////////////////////////////////////////////
//	Класс управления вектором прерываний
//	MSP430F5659A
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_MSP430F5659_H
#define INTERRUPTMAP_MSP430F5659_H

#include "..\Platform.h"
#include "..\EventDispetcher.h"
#include ".\uart.h"
#include ".\i2c.h"
#include "spi.h"
#include "..\msp430\InterruptsState.h"

namespace Rblib
{
namespace Msp430f5659
{
	class InterruptMap : public Msp430::InterrupsState<RBLIB_PALTFORM_AUTODETECT_ID>
	{
	public:

// Вектора прерываний не должны перекрываться,
// поэтому объявляем их только если выбрана данная платформа
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5659

		// USCI B2 Receive/Transmit
		RBLIB_ISR(35 * 2u)
		static void IsrUsciB2()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем

			if (I2CHardware::_C::IsI2CMode())
			{
				// I2C
				I2CHardware::_C::InterruptHandler();
			}
			else
			{
				// SPI
				SpiHardware::_F::RxHandler();
				SpiHardware::_F::TxHandler();
			}
		}

		// USCI A2 Receive/Transmit
		RBLIB_ISR(36 * 2u)
		static void IsrUsciA2()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем

			if (UartHardware::_C::IsUartMode())
			{
				// UART
				UartHardware::_C::RxHandler();
				UartHardware::_C::TxHandler();
			}
			else
			{
				// SPI
				SpiHardware::_E::RxHandler();
				SpiHardware::_E::TxHandler();				
			}
		}

		// Port 4
		RBLIB_ISR(37 * 2u)
		static void IsrPort4()
		{
			RESET_WAKE_UP;
			for (unsigned char i = 0; i < 8; i++)
			{
				if (Gpio::Port4::GetInterruptFlag(i))
				{
					Event e(EventGpioPinChanged, Gpio::Port4::Id, i, Gpio::Port4::Read(i) > 0 ? 1 : 0);
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
			}
			Gpio::Port4::SetInterruptFlags(0x00);
			CHECK_WAKE_UP;
		}

		// Port 3
		RBLIB_ISR(38 * 2u)
		static void IsrPort3()
		{
			RESET_WAKE_UP;
			for (unsigned char i = 0; i < 8; i++)
			{
				if (Gpio::Port3::GetInterruptFlag(i))
				{
					Event e(EventGpioPinChanged, Gpio::Port3::Id, i, Gpio::Port3::Read(i) > 0 ? 1 : 0);
					Singletone<EventDispetcher>::Instance().PushEvent(e);
				}
			}
			Gpio::Port3::SetInterruptFlags(0x00);
			CHECK_WAKE_UP;
		}

		// TA2 CC1-2
		RBLIB_ISR(39 * 2u)
		static void IsrTimerA2Minor()
		{
			RESET_WAKE_UP;
			unsigned short vector = Timer::_D::GetInterruptVector();
			if (vector == 0x0E)
			{
				Event e(EventTimerOverflow, Timer::_D::Id);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			else
			{
				unsigned char regNumber = vector >> 1;
				Event e(EventTimerCompare, Timer::_D::Id, regNumber);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			CHECK_WAKE_UP;
		}

		// TA2 CC0
		RBLIB_ISR(40 * 2u)
		static void IsrTimerA2Main()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_D::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// DAC12_A
		RBLIB_ISR(41 * 2u)
		static void IsrDac12A()
		{
//			RESET_WAKE_UP;
//			Event e(EventTimerCompare, Timer::_D::Id, 0);
//			Singletone<EventDispetcher>::Instance().PushEvent(e);
//			CHECK_WAKE_UP;
		}

		// RTC
		RBLIB_ISR(42 * 2u)
		static void IsrRtc()
		{
		}

		// Port 2
		RBLIB_ISR(44 * 2u)
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

		// USCI B1 Receive/Transmit
		RBLIB_ISR(45 * 2u)
		static void IsrUsciB1()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			if (I2CHardware::_B::IsI2CMode())
			{
				// I2C
				I2CHardware::_B::InterruptHandler();
			}
			else
			{
				// SPI
				SpiHardware::_D::RxHandler();
				SpiHardware::_D::TxHandler();				
			}
		}

		// USCI A1 Receive/Transmit
		RBLIB_ISR(46 * 2u)
		static void IsrUsciA1()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			if (UartHardware::_B::IsUartMode())
			{
				// UART
				UartHardware::_B::RxHandler();
				UartHardware::_B::TxHandler();
			}
			else
			{
				// SPI
				SpiHardware::_C::RxHandler();
				SpiHardware::_C::TxHandler();				
			}
		}

		// Port 1
		RBLIB_ISR(47 * 2u)
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

		// TA1 CC1-2
		RBLIB_ISR(48 * 2u)
		static void IsrTimerA1Minor()
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

		// TA1 CC0
		RBLIB_ISR(49 * 2u)
		static void IsrTimerA1Main()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_B::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// DMA
		RBLIB_ISR(50 * 2u)
		static void IsrDMA()
		{
		}

		// USB_UBM/LDO_PWR
		RBLIB_ISR(51 * 2u)
		static void IsrLdoPwr()
		{
		}

		// TA0 CC1-4
		RBLIB_ISR(52 * 2u)
		static void IsrTimerA0Minor()
		{
			RESET_WAKE_UP;
			unsigned short vector = Timer::_A::GetInterruptVector();
			if (vector == 0x0E)
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

		// TA0 CC0
		RBLIB_ISR(53 * 2u)
		static void IsrTimerA0Main()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_A::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// ADC
		RBLIB_ISR(54 * 2u)
		static void IsrAdc()
		{
		}

		// USCI B0 Receive/Transmit
		RBLIB_ISR(55 * 2u)
		static void IsrUsciB0()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			if (I2CHardware::_A::IsI2CMode())
			{
				// I2C
				I2CHardware::_A::InterruptHandler();
			}
			else
			{
				// SPI
				SpiHardware::_B::RxHandler();
				SpiHardware::_B::TxHandler();				
			}
		}

		// USCI A0 Receive/Transmit
		RBLIB_ISR(56 * 2u)
		static void IsrUsciA0()
		{
			// т.к. модули USCI могут работать в нескольких режимах, проверяем в каком режиме работаем
			
			if (UartHardware::_A::IsUartMode())
			{
				// UART
				UartHardware::_A::RxHandler();
				UartHardware::_A::TxHandler();
			}
			else
			{
				// SPI
				SpiHardware::_A::RxHandler();
				SpiHardware::_A::TxHandler();				
			}
		}

		// Watchdog Timer
		RBLIB_ISR(57 * 2u)
		static void IsrWdt()
		{
		}

		// TB0 CC1-6
		RBLIB_ISR(58 * 2u)
		static void IsrTimerB0Minor()
		{
			RESET_WAKE_UP;
			unsigned short vector = Timer::_C::GetInterruptVector();
			if (vector == 0x0E)
			{
				Event e(EventTimerOverflow, Timer::_C::Id);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			else
			{
				unsigned char regNumber = vector >> 1;
				Event e(EventTimerCompare, Timer::_C::Id, regNumber);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
			CHECK_WAKE_UP;
		}

		// TB0 CC0
		RBLIB_ISR(59 * 2u)
		static void IsrTimerB0Main()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_C::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// Comp_B
		RBLIB_ISR(60 * 2u)
		static void IsrCompB()
		{
		}

		// User Non-maskable
		RBLIB_ISR(61 * 2u)
		static void IsrNmiUser()
		{
		}

		// System Non-maskable
		RBLIB_ISR(62 * 2u)
		static void IsrNmiSystem()
		{
		}

#endif // RBLIB_PLATFORM_ID_MSP430F5659

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

}
}

#endif // INTERRUPTMAP_MSP430F5659_H
