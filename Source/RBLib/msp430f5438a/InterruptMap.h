///////////////////////////////////////////////////////////////////////////////
//	����� ���������� �������� ����������
//	MSP430F5438A
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPTMAP_MSP430F5438A_H
#define INTERRUPTMAP_MSP430F5438A_H

#include "..\Platform.h"
#include "..\EventDispetcher.h"
#include ".\uart.h"
#include ".\I2C.h"
#include ".\system\OperatingModeWakeUp.h"
#include "..\msp430\InterruptsState.h"
#include "..\Timer.h"

namespace Rblib
{
namespace Msp430f5438a
{
	class InterruptMap : public Msp430::InterrupsState<RBLIB_PALTFORM_AUTODETECT_ID>
	{
	public:
		// ������� ���������� �� ������ �������������,
		// ������� ��������� �� ������ ���� ������� ������ ���������
		#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5438A
		// RTC
		RBLIB_ISR(41 * 2u)
		static void IsrRtc()
		{
			
		}

		// Port 2
		RBLIB_ISR(42 * 2u)
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

		// USCI B3 Receive/Transmit
		RBLIB_ISR(43 * 2u)
		static void IsrUsciB3()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
			if (I2CHardware::_D::IsI2CMode())
			{
				// I2C
				I2CHardware::_D::InterruptHandler();
			}
			else
			{
				// SPI
			}			
		}

		// USCI A3 Receive/Transmit
		RBLIB_ISR(44 * 2u)
		static void IsrUsciA3()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
			if (UartHardware::_D::IsUartMode())
			{
				// UART
				UartHardware::_D::RxHandler();
				UartHardware::_D::TxHandler();
			}
			else
			{
				// SPI
			}
		}

		// USCI B1 Receive/Transmit
		RBLIB_ISR(45 * 2u)
		static void IsrUsciB1()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
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

		// USCI A1 Receive/Transmit
		RBLIB_ISR(46 * 2u)
		static void IsrUsciA1()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
			if (UartHardware::_B::IsUartMode())
			{
				// UART
				UartHardware::_B::RxHandler();
				UartHardware::_B::TxHandler();
			}
			else
			{
				// SPI
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

		// Timer1_A3 CC1-2, TA1
		RBLIB_ISR(48 * 2u)
		static void IsrTimer1A1()
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

		// Timer1_A3 CC0
		RBLIB_ISR(49 * 2u)
		static void IsrTimer1A0()
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

		// USCI B2 Receive/Transmit
		RBLIB_ISR(51 * 2u)
		static void IsrUsciB2()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
			if (I2CHardware::_C::IsI2CMode())
			{
				// I2C
				I2CHardware::_C::InterruptHandler();
			}
			else
			{
				// SPI
			}
		}

		// USCI A2 Receive/Transmit
		RBLIB_ISR(52 * 2u)
		static void IsrUsciA2()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
			if (UartHardware::_C::IsUartMode())
			{
				// UART
				UartHardware::_C::RxHandler();
				UartHardware::_C::TxHandler();
			}
			else
			{
				// SPI
			}
		}

		// Timer0_A5 CC1-4, TA
		RBLIB_ISR(53 * 2u)
		static void IsrTimer0A1()
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

		// Timer0_A5 CC0
		RBLIB_ISR(54 * 2u)
		static void IsrTimer0A0()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_A::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
		}

		// ADC
		RBLIB_ISR(55 * 2u)
		static void IsrAdc()
		{
			
		}

		// USCI B0 Receive/Transmit
		RBLIB_ISR(56 * 2u)
		static void IsrUsciB0()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
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

		// USCI A0 Receive/Transmit
		RBLIB_ISR(57 * 2u)
		static void IsrUsciA0()
		{
			// �.�. ������ USCI ����� �������� � ���������� �������, ��������� � ����� ������ ��������
			
			if (UartHardware::_A::IsUartMode())
			{
				// UART
				UartHardware::_A::RxHandler();
				UartHardware::_A::TxHandler();
			}
			else
			{
				// SPI
			}
		}

		// Watchdog Timer
		RBLIB_ISR(58 * 2u)
		static void IsrWdt()
		{
			
		}

		// Timer B CC1-2, TB
		RBLIB_ISR(59 * 2u)
		static void IsrTimerB1()
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

		// Timer B CC0
		RBLIB_ISR(60 * 2u)
		static void IsrTimerB0()
		{
			RESET_WAKE_UP;
			Event e(EventTimerCompare, Timer::_C::Id, 0);
			Singletone<EventDispetcher>::Instance().PushEvent(e);
			CHECK_WAKE_UP;
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
		#endif

		// ��������� ����������
		static inline void GlobalInterruptEnable(void)
		{
			RBLIB_ASM("eint");
		}

		// ��������� ����������
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