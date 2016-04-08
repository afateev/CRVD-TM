
#ifndef BOARD_H
#define BOARD_H

#include "../RbLib/Rblib.h"
#include "Rs485InterfaceImpl.h"

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
		typedef Rblib::Afio Afio;
		
		class Clock
		{
		public:
			static const unsigned int HSEClockFrequency = 8000000; //MHz
		public:
			static void Init()
			{
				// HSE - внешнее тактирование с частотой HSEClockFrequency
				Rcc::HSEEnable();
				while(!Rcc::HSIReady())
				{
					;
				}
				
				// PLL от HSE
				Rcc::SetPLLSource(Rcc::PLLSourceHSE);
				// HSE без делени€
				Rcc::SetPLLHSEDivider(Rcc::HSEDiv1);
				// PLL = 72 MHz = 8 * 9
				Rcc::SetPLLMultiplicationFactor(Rcc::PLLMul9);
				// ¬ключаем
				Rcc::PLLEnable();
				while(!Rcc::PLLReady())
				{
					;
				}
				
				// Two wait states, if 48 MHz < SYSCLK <= 72 MHz
				FlashMemoryController::SetLatency(2);
				
				// max 36 MHz
				Rcc::SetAPB1Prescaler(Rcc::APB1PrescalerHCLKDiv2);
				
				// ядро от PLL
				Rcc::SetSystemClock(Rcc::SystemClockPLL);
				
				Rcc::EnableClockAfio();
				Rcc::EnableClockPortA();
				Rcc::EnableClockPortC();
				Rcc::EnableClockPortD();
				Rcc::EnableClockPortE();
				Rcc::EnableClockTimer1();
				Rcc::EnableClockTimer2();
				Rcc::EnableClockPowerInterface();
				Rcc::EnableClockUsart2();
				Rcc::EnableClockUsart3();
				Rcc::EnableClockSpi3();
			}
		};
		
		class DisplayConnection
		{
		public:
			typedef Rblib::Gpio::E Port;
			
			// структура порта на который присоединен дисплей
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
				DataType DataReadWrite	: 1;	// 0 - Data на вход, 1 - Data на выход
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
		
		class ControllerSwitchConnection
		{
		public:
			typedef Gpio::A Port;
			static const int Pin1 = 4;
			static const int Pin2 = 5;
		};
		
		class PortScanerConnection
		{
		public:
			typedef Gpio::D Port;
			static const int Pin0 = 10;
			static const int Pin1 = 11;
		};
		
		typedef Timer::Timer1 TenKiloHertzTimer;
		static const unsigned int TenKiloHertzTickFrequency = 10000;
		
		typedef Timer::Timer2 KiloHertzTimer;
		static const unsigned int KiloHertzTickFrequency = 1000;
		
		typedef Usart::Usart2 MainComPort;
		
		typedef Rs485InterfaceImpl<Usart::Usart2, Gpio::D, 7> Rs485Interface;
	public:
		static void Init()
		{
			Clock::Init();
			DisplayConnection::Init();
			
			TenKiloHertzTimer::SetPrescaler(72);
			TenKiloHertzTimer::SetMaxCount(1000000 / TenKiloHertzTickFrequency);
			TenKiloHertzTimer::Start();
			TenKiloHertzTimer::ClearUpdateInterruptFlag();
			TenKiloHertzTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM1_UP);
			
			KiloHertzTimer::SetPrescaler(72);
			KiloHertzTimer::SetMaxCount(1000000 / TenKiloHertzTickFrequency);
			KiloHertzTimer::Start();
			KiloHertzTimer::ClearUpdateInterruptFlag();
			KiloHertzTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM2);
			
			Gpio::D::SetMode(8, Gpio::D::ModeOutput);
			Gpio::D::SetConfig(8, Gpio::D::ConfigOutputAlternatePushPull);
			Afio::RemapUsart3(3);
			
			MainComPort::Init(36000000, 9600, MainComPort::StopBits1, MainComPort::ParityNone);
			//MainComPort::OnRead = OnUsartRead;
			//MainComPort::OnWriteComplete = OnWriteComplete;
			MainComPort::Enable();
			
			// 485
			// RE/DE
			Gpio::D::SetMode(7, Gpio::D::ModeOutput);
			Gpio::D::SetConfig(7, Gpio::D::ConfigOutputPushPull);
			Gpio::D::ClearBit(7);
		}
		
		static void OnTimer1Update()
		{
		}
	};
}

#endif