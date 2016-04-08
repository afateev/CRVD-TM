#ifndef INTERRUPTMAP_EFM32_H
#define INTERRUPTMAP_EFM32_H

#include "../Utils/CallbackListWrapper.h"
#include "../Gpio.h"
#include "../Uart.h"
#include "../Timer.h"
#include "../SpiMaster.h"
#include "../i2c.h"
#include "../Dma.h"
#include "../SystemTimer.h"

namespace Rblib
{
namespace Efm32
{
	#pragma segment="CSTACK"
	extern "C" void __iar_program_start( void );
	
	template
		<
			int platformId = RBLIB_PALTFORM_AUTODETECT_ID
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
		
		typedef Rblib::Gpio Gpio;
		typedef Rblib::Uart Uart;
		typedef Rblib::Timer Timer;
		typedef Rblib::I2C I2C;
		typedef Rblib::Dma Dma;
		typedef Rblib::SystemTimer SystemTimer;
		
		class InterruptVectorTable
		{
		public:
			static const int ExceptionCount = 16;
			static const int IrqCount = 38;
			static const int AlignmentPadding = 32;
			static const int VectorTableItemsCount = ExceptionCount + IrqCount + AlignmentPadding;
			
			typedef void( *FunctionPtrType )( void );
			union VectorTableItem
			{
				FunctionPtrType FunctionPtr;
				void * Ptr;
			};
		protected:
			VectorTableItem _vectorTable[VectorTableItemsCount];
			int _offset;
		public:
			InterruptVectorTable()
			{
				_offset = 0;
				int i = 0;
				
				// начальный адрес таблицы должен быть кратным 128 байт
				// ищем начальное смещение при котором адрес будет кратным
				for (i = 0; i < AlignmentPadding; i++)
				{
					unsigned int addr = (unsigned int)&_vectorTable[i];
					if (addr % 128 == 0)
					{
						_offset = i;
						break;
					}
				}
				_vectorTable[i++].Ptr = __sfe( "CSTACK" );
				_vectorTable[i++].FunctionPtr = __iar_program_start;
				_vectorTable[i++].FunctionPtr = NMI;
				_vectorTable[i++].FunctionPtr = HardFault;
				_vectorTable[i++].FunctionPtr = MemoryManagementFault;
				_vectorTable[i++].FunctionPtr = BusFault;
				_vectorTable[i++].FunctionPtr = UsageFault;
				_vectorTable[i++].FunctionPtr = 0;
				_vectorTable[i++].FunctionPtr = 0;
				_vectorTable[i++].FunctionPtr = 0;
				_vectorTable[i++].FunctionPtr = 0;
				_vectorTable[i++].FunctionPtr = SVCall;
				_vectorTable[i++].FunctionPtr = 0;
				_vectorTable[i++].FunctionPtr = 0;
				_vectorTable[i++].FunctionPtr = PendSV;
				_vectorTable[i++].FunctionPtr = SystemTick;
				_vectorTable[i++].FunctionPtr = DMA;
				_vectorTable[i++].FunctionPtr = GPIO_EVEN;
				_vectorTable[i++].FunctionPtr = TIMER0;
				_vectorTable[i++].FunctionPtr = USART0_RX;
				_vectorTable[i++].FunctionPtr = USART0_TX;
				_vectorTable[i++].FunctionPtr = USB;
				_vectorTable[i++].FunctionPtr = ACMP0_ACMP1;
				_vectorTable[i++].FunctionPtr = ADC0;
				_vectorTable[i++].FunctionPtr = DAC0;
				_vectorTable[i++].FunctionPtr = I2C0;
				_vectorTable[i++].FunctionPtr = I2C1;
				_vectorTable[i++].FunctionPtr = GPIO_ODD;
				_vectorTable[i++].FunctionPtr = TIMER1;
				_vectorTable[i++].FunctionPtr = TIMER2;
				_vectorTable[i++].FunctionPtr = TIMER3;
				_vectorTable[i++].FunctionPtr = USART1_RX;
				_vectorTable[i++].FunctionPtr = USART1_TX;
				_vectorTable[i++].FunctionPtr = LESENSE;
				_vectorTable[i++].FunctionPtr = USART2_RX;
				_vectorTable[i++].FunctionPtr = USART2_TX;
				_vectorTable[i++].FunctionPtr = UART0_RX;
				_vectorTable[i++].FunctionPtr = UART0_TX;
				_vectorTable[i++].FunctionPtr = UART1_RX;
				_vectorTable[i++].FunctionPtr = UART1_TX;
				_vectorTable[i++].FunctionPtr = LEUART0;
				_vectorTable[i++].FunctionPtr = LEUART1;
				_vectorTable[i++].FunctionPtr = LETIMER0;
				_vectorTable[i++].FunctionPtr = PCNT0;
				_vectorTable[i++].FunctionPtr = PCNT1;
				_vectorTable[i++].FunctionPtr = PCNT2;
				_vectorTable[i++].FunctionPtr = RTC;
				_vectorTable[i++].FunctionPtr = BURTC;
				_vectorTable[i++].FunctionPtr = CMU;
				_vectorTable[i++].FunctionPtr = VCMP;
				_vectorTable[i++].FunctionPtr = LCD;
				_vectorTable[i++].FunctionPtr = MSC;
				_vectorTable[i++].FunctionPtr = AES;
				_vectorTable[i++].FunctionPtr = EBI;
			}
			
			void *VectorTable()
			{
				return &_vectorTable[_offset];
			}
		};
	public:
		static UndefinedInterruptHandlerType UndefinedInterruptHandler;
		static FaultHandlerType FaultHandler;
	protected:
		static InterruptVectorTable _interruptVectorTable;
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
			SystemTimer::InterruptHandler();
		}
		
		static void DMA()
		{
			Dma::InterruptHandler();
		}
		
		static void GPIO_EVEN()
		{
			Gpio::InterruptHandler(false);
		}
		
		static void TIMER0()
		{
			Timer::TIMER0::InterruptHandler();
		}
		
		static void USART0_RX()
		{
			if (Uart::USART0::IsSynchronousMode())
			{
				// SPI
				SpiMaster::USART0::RxInterruptHandler();
			}
			else
			{
				// UART
				Uart::USART0::RxInterruptHandler();
			}
		}
		
		static void USART0_TX()
		{
			if (Uart::USART0::IsSynchronousMode())
			{
				// SPI
				SpiMaster::USART0::TxInterruptHandler();
			}
			else
			{
				// UART
				Uart::USART0::TxInterruptHandler();
			}
		}
		
		static void USB()
		{
			UndefinedInterruptHandler.Call(5);
		}
		
		static void ACMP0_ACMP1()
		{
			UndefinedInterruptHandler.Call(6);
		}
		
		static void ADC0()
		{
			UndefinedInterruptHandler.Call(7);
		}
		
		static void DAC0()
		{
			UndefinedInterruptHandler.Call(8);
		}
		
		static void I2C0()
		{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_EFM32GG990F1024
			I2C::A::InterruptHandler();
#endif
		}
		
		static void I2C1()
		{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_EFM32GG990F1024
            I2C::B::InterruptHandler();
#endif
		}
		
		static void GPIO_ODD()
		{
			Gpio::InterruptHandler(true);
		}
		
		static void TIMER1()
		{
			Timer::TIMER1::InterruptHandler();
		}
		
		static void TIMER2()
		{
			Timer::TIMER2::InterruptHandler();
		}
		
		static void TIMER3()
		{
			Timer::TIMER3::InterruptHandler();
		}
		
		static void USART1_RX()
		{
			if (Uart::USART1::IsSynchronousMode())
			{
				// SPI
				SpiMaster::USART1::RxInterruptHandler();
			}
			else
			{
				// UART
				Uart::USART1::RxInterruptHandler();
			}
		}
		
		static void USART1_TX()
		{
			if (Uart::USART1::IsSynchronousMode())
			{
				// SPI
				SpiMaster::USART1::TxInterruptHandler();
			}
			else
			{
				// UART
				Uart::USART1::TxInterruptHandler();
			}
		}
		
		static void LESENSE()
		{
			UndefinedInterruptHandler.Call(17);
		}
		
		static void USART2_RX()
		{
			if (Uart::USART2::IsSynchronousMode())
			{
				// SPI
				SpiMaster::USART2::RxInterruptHandler();
			}
			else
			{
				// UART
				Uart::USART2::RxInterruptHandler();
			}
		}
		
		static void USART2_TX()
		{
			if (Uart::USART2::IsSynchronousMode())
			{
				// SPI
				SpiMaster::USART2::TxInterruptHandler();
			}
			else
			{
				// UART
				Uart::USART2::TxInterruptHandler();
			}
		}
		
		static void UART0_RX()
		{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_EFM32GG990F1024
			Uart::UART0::RxInterruptHandler();
#endif
		}
		
		static void UART0_TX()
		{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_EFM32GG990F1024
			Uart::UART0::TxInterruptHandler();
#endif
		}
		
		static void UART1_RX()
		{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_EFM32GG990F1024
			Uart::UART1::RxInterruptHandler();
#endif
		}
		
		static void UART1_TX()
		{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_EFM32GG990F1024
			Uart::UART1::TxInterruptHandler();
#endif
		}
		
		static void LEUART0()
		{
		}
		
		static void LEUART1()
		{
		}
		
		static void LETIMER0()
		{
			Efm32gg::LeTimer::LETIMER0::InterruptHandler();
		}
		
		static void PCNT0()
		{
			UndefinedInterruptHandler.Call(27);
		}
		
		static void PCNT1()
		{
			UndefinedInterruptHandler.Call(28);
		}
		
		static void PCNT2()
		{
			UndefinedInterruptHandler.Call(29);
		}
		
		static void RTC()
		{
			Efm32gg::RealTimeCounter::Rtc::InterruptHandler();
		}
		
		static void BURTC()
		{
			UndefinedInterruptHandler.Call(31);
		}
		
		static void CMU()
		{
			UndefinedInterruptHandler.Call(32);
		}
		
		static void VCMP()
		{
			UndefinedInterruptHandler.Call(33);
		}
		
		static void LCD()
		{
			UndefinedInterruptHandler.Call(34);
		}
		
		static void MSC()
		{
			UndefinedInterruptHandler.Call(35);
		}
		
		static void AES()
		{
			UndefinedInterruptHandler.Call(36);
		}
		
		static void EBI()
		{
			UndefinedInterruptHandler.Call(37);
		}
		
		// Разрешить прерывания
		static inline void GlobalInterruptEnable(void)
		{
			RBLIB_GLOBAL_INTERRUPT_ENABLE;
		}

		// Запретить прерывания
		static inline void GlobalInterruptDisable(void)
		{
			RBLIB_GLOBAL_INTERRUPT_DISABLE;
		}
		
		static void *GetVectorTable()
		{
			return _interruptVectorTable.VectorTable();
		}
	};
	
	template < int platformId >
		InterruptMap<platformId>::UndefinedInterruptHandlerType InterruptMap<platformId>::UndefinedInterruptHandler;
	
	template < int platformId >
		InterruptMap<platformId>::FaultHandlerType InterruptMap<platformId>::FaultHandler;
	
	// Динамический вектор прерываний, хранится в RAM
	// Создается при старте программы к конструкторе InterruptVectorTable
	template < int platformId >
		InterruptMap<platformId>::InterruptVectorTable InterruptMap<platformId>::_interruptVectorTable;
	
	typedef void( *intfunc )( void );
	typedef union { intfunc __fun; void * __ptr; } intvec_elem;
	
	// константный вектор прерываний
	// Создается во время линковки, хранится в Flash
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
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DMA,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::GPIO_EVEN,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIMER0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART0_RX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART0_TX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USB,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::ACMP0_ACMP1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::ADC0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::DAC0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::I2C1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::GPIO_ODD,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIMER1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIMER2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::TIMER3,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART1_RX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART1_TX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::LESENSE,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART2_RX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::USART2_TX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART0_RX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART0_TX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART1_RX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::UART1_TX,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::LEUART0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::LEUART1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::LETIMER0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::PCNT0,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::PCNT1,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::PCNT2,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::RTC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::BURTC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::CMU,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::VCMP,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::LCD,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::MSC,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::AES,
			InterruptMap<RBLIB_PALTFORM_AUTODETECT_ID>::EBI
		};
}
}

#endif