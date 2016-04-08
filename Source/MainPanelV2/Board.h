
#ifndef BOARD_H
#define BOARD_H

#include "../RbLib/Rblib.h"

namespace MainPanel
{
	class Board
	{
	public:
		typedef Rblib::ResetAndClockControl Rcc;
		typedef Rblib::Gpio Gpio;
		typedef Rblib::Timer Timer;
		typedef Rblib::Nvic Nvic;
		typedef Rblib::InterruptMap InterruptMap;
		typedef Rblib::Usart Usart;
		typedef Rblib::FlashMemoryController FlashMemoryController;
		
		class Clock
		{
		public:
			static const unsigned int HSEClockFrequency = 8000000; //MHz
		public:
			static void Init()
			{
				// HSE - ������� ������������ � �������� HSEClockFrequency
				Rcc::HSEEnable();
				while(!Rcc::HSIReady())
				{
					;
				}
				
				// PLL �� HSE
				Rcc::SetPLLSource(Rcc::PLLSourceHSE);
				// HSE ��� �������
				Rcc::SetPLLHSEDivider(Rcc::HSEDiv1);
				// PLL = 72 MHz = 8 * 9
				Rcc::SetPLLMultiplicationFactor(Rcc::PLLMul9);
				// ��������
				Rcc::PLLEnable();
				while(!Rcc::PLLReady())
				{
					;
				}
				
				// Two wait states, if 48 MHz < SYSCLK <= 72 MHz
				FlashMemoryController::SetLatency(2);
				
				// max 36 MHz
				Rcc::SetAPB1Prescaler(Rcc::APB1PrescalerHCLKDiv2);
				
				// ���� �� PLL
				Rcc::SetSystemClock(Rcc::SystemClockPLL);
				
				Rcc::EnableClockAfio();
				Rcc::EnableClockPortA();
				Rcc::EnableClockPortC();
				Rcc::EnableClockPortD();
				Rcc::EnableClockPortE();
				Rcc::EnableClockUsart2();
				Rcc::EnableClockUsart3();
				Rcc::EnableClockSpi3();
			}
		};
		
		class DisplayConnection
		{
		public:
			typedef Rblib::Gpio::E Port;
			
			// ��������� ����� �� ������� ����������� �������
			template<class DataType>
			struct DisplayInterfacePortStruct
			{
				DataType Data 			: 8;
				DataType A0				: 1;
				DataType CS				: 1;
				DataType RW				: 1;
				DataType E				: 1;
				DataType Reserved0		: 1;
				DataType Reset			: 1;
				DataType DataReadWrite	: 1;	// 0 - Data �� ����, 1 - Data �� �����
				DataType Reserved1		: 1;
				
				typedef DataType InternalType;
			};
			
			typedef DisplayInterfacePortStruct<unsigned short> PortStruct;
		public:
			static void Init()
			{
				for (int i = 0; i < 16; i++)
				{
					Port::SetMode(i, Port::ModeOutput);
					Port::SetConfig(i, Port::ConfigOutputPushPull);
				}
			}
		};
		
	public:
		static void Init()
		{
			Clock::Init();
			DisplayConnection::Init();
		}
	};
}

#endif