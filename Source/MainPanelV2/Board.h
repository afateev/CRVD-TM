
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
				// HSE без деления
				Rcc::SetPLLHSEDivider(Rcc::HSEDiv1);
				// PLL = 72 MHz = 8 * 9
				Rcc::SetPLLMultiplicationFactor(Rcc::PLLMul9);
				// Включаем
				Rcc::PLLEnable();
				while(!Rcc::PLLReady())
				{
					;
				}
				
				// Two wait states, if 48 MHz < SYSCLK <= 72 MHz
				FlashMemoryController::SetLatency(2);
				
				// max 36 MHz
				Rcc::SetAPB1Prescaler(Rcc::APB1PrescalerHCLKDiv2);
				
				// Ядро от PLL
				Rcc::SetSystemClock(Rcc::SystemClockPLL);
				
				// Вывод тактирования для проверки
				Rcc::SetClockOutput(Rcc::ClockOutputPllDiv2);
				
				Rcc::EnableClockAfio();
				Rcc::EnableClockPortA();
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
				
				//Вывод тактирования
				Gpio::A::SetMode(8, Gpio::A::ModeOutput);
				//Gpio::A::SetConfig(8, Gpio::A::ConfigOutputAlternatePushPull);
				Gpio::A::SetConfig(8, Gpio::A::ConfigOutputPushPull);
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
			
			TenKiloHertzTimer::SetPrescaler(72);
			TenKiloHertzTimer::SetMaxCount(1000000 / TenKiloHertzTickFrequency);
			TenKiloHertzTimer::Start();
			TenKiloHertzTimer::ClearUpdateInterruptFlag();
			TenKiloHertzTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM1_UP);
			
			KiloHertzTimer::SetPrescaler(72);
			KiloHertzTimer::SetMaxCount(1000000 / KiloHertzTickFrequency);
			KiloHertzTimer::Start();
			KiloHertzTimer::ClearUpdateInterruptFlag();
			KiloHertzTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM2);
			Nvic::SetInterruptPriority(Nvic::InterruptVector_TIM2, 2);
			
			GuiTimer::SetPrescaler(720);
			GuiTimer::SetMaxCount(100000 / GuiTickFrequency);
			GuiTimer::Start();
			GuiTimer::ClearUpdateInterruptFlag();
			GuiTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM3);
			Nvic::SetInterruptPriority(Nvic::InterruptVector_TIM3, 4);
			
			ModbusSlaveTimer::SetPrescaler(720);
			ModbusSlaveTimer::SetMaxCount(100000 / ModbusSlaveFrequency);
			ModbusSlaveTimer::Start();
			ModbusSlaveTimer::ClearUpdateInterruptFlag();
			ModbusSlaveTimer::EnableUpdateInterrupt();
			Nvic::InterruptEnable(Nvic::InterruptVector_TIM4);
			Nvic::SetInterruptPriority(Nvic::InterruptVector_TIM4, 3);
			
			Gpio::D::SetMode(8, Gpio::D::ModeOutput);
			Gpio::D::SetConfig(8, Gpio::D::ConfigOutputAlternatePushPull);
			Afio::RemapUsart3(3);
			
			MainComPort::Init(36000000, 9600, MainComPort::StopBits1, MainComPort::ParityNone);
			//MainComPort::OnRead = OnUsartRead;
			//MainComPort::OnWriteComplete = OnWriteComplete;
			MainComPort::Enable();
			Nvic::InterruptEnable(Nvic::InterruptVector_USART3);
			
			// 485
			// RE/DE
			Gpio::D::SetMode(7, Gpio::D::ModeOutput);
			Gpio::D::SetConfig(7, Gpio::D::ConfigOutputPushPull);
			Gpio::D::ClearBit(7);
			// TX
			Gpio::D::SetMode(5, Gpio::D::ModeOutput);
			Gpio::D::SetConfig(5, Gpio::D::ConfigOutputAlternatePushPull);
			Afio::RemapUsart2(1);
			Nvic::InterruptEnable(Nvic::InterruptVector_USART2);
			Rs485Init(9600, false, false);
			
			// SPI3
			// CLK as alternate output
			Gpio::C::SetMode(10, Gpio::C::ModeOutput);
			Gpio::C::SetConfig(10, Gpio::C::ConfigOutputAlternatePushPull);
			// MOSI as alternate output
			Gpio::C::SetMode(12, Gpio::C::ModeOutput);
			Gpio::C::SetConfig(12, Gpio::C::ConfigOutputAlternatePushPull);
			Afio::RemapSpi3(1);
			
			// 485 Diagnostics Master
			// RE/DE
			Gpio::B::SetMode(7, Gpio::B::ModeOutput);
			Gpio::B::SetConfig(7, Gpio::B::ConfigOutputPushPull);
			Gpio::B::ClearBit(7);
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