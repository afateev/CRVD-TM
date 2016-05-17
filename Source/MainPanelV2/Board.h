
#ifndef BOARD_H
#define BOARD_H

#include "../RbLib/Rblib.h"
#include "Rs485InterfaceImpl.h"
#include "Config.h"

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
				
				// VCO input frequency = PLL input clock frequency / PLLM
				// It is recommended to select a frequency of 2 MHz
				// VCO input frequency = 8 / 4 = 2 MHz
				Rcc::SetPLLM(4);
				
				// VCO output frequency = VCO input frequency * PLLN with 50 <= PLLN <= 432
				// The software has to set these bits correctly to ensure that the VCO output frequency is between 100 and 432 MHz
				// VCO output frequency = 2 * 168 = 336 MHz
				Rcc::SetPLLN(168);
				
				// PLL output clock frequency = VCO frequency / PLLP
				// PLL output clock frequency = 336 / 2 = 168 MHz
				Rcc::SetPLLP(2);
				
				// USB OTG FS clock frequency = VCO frequency / PLLQ with 2 <= PLLQ <= 15
				// USB OTG FS clock frequency = 336 / 7 = 48 MHz
				Rcc::SetPLLQ(7);
				
				// ¬ключаем
				Rcc::PLLEnable();
				
				while(!Rcc::PLLReady())
				{
					;
				}
				
				// max 36 MHz
				// 168 / 8 = 21 MHz
				Rcc::SetAPB1Prescaler(Rcc::APB1PrescalerAHBDiv8);
				
				// max 72 MHz
				// 168 / 8 = 42 MHz
				Rcc::SetAPB2Prescaler(Rcc::APB2PrescalerAHBDiv4);
				
				// 5 wait states, if 150 MHz < SYSCLK <= 168 MHz
				FlashMemoryController::SetLatency(5);
				
				// ядро от PLL
				Rcc::SetSystemClock(Rcc::SystemClockPLL);
				
				Rcc::EnableClockPortA();
				Rcc::EnableClockPortB();
				Rcc::EnableClockPortC();
				Rcc::EnableClockPortD();
				Rcc::EnableClockPortE();
				Rcc::EnableClockTimer1();
				Rcc::EnableClockTimer2();
				Rcc::EnableClockTimer3();
				Rcc::EnableClockTimer4();
				Rcc::EnableClockPowerInterface();
				Rcc::EnableClockUsart2();
				Rcc::EnableClockUsart3();
				Rcc::EnableClockSpi3();
				Rcc::EnableClockPowerInterface();
				
				if (!Rcc::IsRtcEnabled())
				{
					PowerControl::DisableBackupDomainWriteProtection(true);
					Rcc::LSEEnable();
					unsigned int waitTimeOut = 1000;
					while(!Rcc::LSEReady() && waitTimeOut > 0)
					{
						waitTimeOut--;
					}
					Rcc::SetRtcSource(Rcc::RtcSourceLSE);
					Rcc::RtcEnable();
					PowerControl::DisableBackupDomainWriteProtection(false);
				}
				
				//¬ывод тактировани€
				Gpio::A::SetMode(8, Gpio::A::ModeOutput);
				//Gpio::A::SetConfig(8, Gpio::A::ConfigOutputAlternatePushPull);
				//Gpio::A::SetBit(8);
				Gpio::A::ClearBit(8);
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
				}
				
				for (int i = 0; i < 16; i++)
				{
					for (int j = 0; j < 16; j++)
					{
						Port::ClearBit(j);
					}
					Port::SetBit(i);
					
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
		
		typedef Timer::Timer3 GuiTimer;
		static const unsigned int GuiTickFrequency = 5;
		
		typedef Timer::Timer4 ModbusSlaveTimer;
		static const unsigned int ModbusSlaveFrequency = 100;
		
		typedef Usart::Usart3 MainComPort;
		
		typedef Rs485InterfaceImpl<Usart::Usart2, Gpio::D, 7> Rs485Interface;
	public:
		static void Init()
		{
			Clock::Init();
			DisplayConnection::Init();
			
			TenKiloHertzTimer::SetPrescaler(42 * 2);
			TenKiloHertzTimer::SetMaxCount(1000000 / TenKiloHertzTickFrequency);
			TenKiloHertzTimer::Start();
			TenKiloHertzTimer::ClearUpdateInterruptFlag();
			TenKiloHertzTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM1_UP);
			
			KiloHertzTimer::SetPrescaler(21 * 2);
			KiloHertzTimer::SetMaxCount(1000000 / KiloHertzTickFrequency);
			KiloHertzTimer::Start();
			KiloHertzTimer::ClearUpdateInterruptFlag();
			KiloHertzTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM2);
			Nvic::SetInterruptPriority(Nvic::InterruptVector_TIM2, 2);
			
			GuiTimer::SetPrescaler(210 * 2);
			GuiTimer::SetMaxCount(100000 / GuiTickFrequency);
			GuiTimer::Start();
			GuiTimer::ClearUpdateInterruptFlag();
			GuiTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM3);
			Nvic::SetInterruptPriority(Nvic::InterruptVector_TIM3, 4);
			
			ModbusSlaveTimer::SetPrescaler(210 * 2);
			ModbusSlaveTimer::SetMaxCount(100000 / ModbusSlaveFrequency);
			ModbusSlaveTimer::Start();
			ModbusSlaveTimer::ClearUpdateInterruptFlag();
			ModbusSlaveTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM4);
			Nvic::SetInterruptPriority(Nvic::InterruptVector_TIM4, 3);
			
			//USART3 AF7
			Gpio::D::SetMode(8, Gpio::D::ModeAlternate);
			Gpio::D::SetAlternateFunctionNumber(8, 7);
			Gpio::D::SetMode(9, Gpio::D::ModeAlternate);
			Gpio::D::SetAlternateFunctionNumber(9, 7);
			
			MainComPort::Init(Config::MainComPortClockSourceFrequency, 9600, MainComPort::StopBits1, MainComPort::ParityNone);
			//MainComPort::OnRead = OnUsartRead;
			//MainComPort::OnWriteComplete = OnWriteComplete;
			MainComPort::Enable();
			Nvic::InterruptEnable(Nvic::InterruptVector_USART3);
			
			// 485
			// RE/DE
			Gpio::D::SetMode(7, Gpio::D::ModeOutput);
			Gpio::D::ClearBit(7);
			//USART3 AF7
			Gpio::D::SetMode(5, Gpio::D::ModeAlternate);
			Gpio::D::SetAlternateFunctionNumber(5, 7);
			Gpio::D::SetMode(6, Gpio::D::ModeAlternate);
			Gpio::D::SetAlternateFunctionNumber(6, 7);
			Nvic::InterruptEnable(Nvic::InterruptVector_USART2);
			Rs485Init(9600, false, false);
			
			// SPI3 AF6
			// CLK as alternate output
			Gpio::C::SetMode(10, Gpio::C::ModeAlternate);
			Gpio::C::SetAlternateFunctionNumber(10, 6);
			// MISO as alternate output
			Gpio::C::SetMode(11, Gpio::C::ModeAlternate);
			Gpio::C::SetAlternateFunctionNumber(11, 6);
			// MOSI as alternate output
			Gpio::C::SetMode(12, Gpio::C::ModeAlternate);
			Gpio::C::SetAlternateFunctionNumber(12, 6);
			
			// 485 Diagnostics Master
			// RE/DE
			Gpio::B::SetMode(7, Gpio::B::ModeOutput);
			Gpio::B::ClearBit(7);
			
			Gpio::B::SetMode(0, Gpio::B::ModeOutput);
			Gpio::B::SetMode(1, Gpio::B::ModeOutput);
		}
		
		static void Rs485Init(int boudrate, bool parityEnable, bool parityEven)
		{
			Rs485Interface::Init(Config::Por485tClockSourceFrequency, boudrate, parityEnable, parityEven);
		}
		
		static void OnTimer1Update()
		{
		}
		
		static void DiagnosticRs485TxEnable(bool enable)
		{
			if (enable)
			{
				Gpio::B::SetBit(7);
			}
			else
			{
				Gpio::B::ClearBit(7);
			}
		}
	};
}

#endif