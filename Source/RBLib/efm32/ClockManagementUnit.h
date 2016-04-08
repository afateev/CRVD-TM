#ifndef CMU_EMF32_H
#define CMU_EMF32_H

#include "Register.h"

namespace Rblib
{
namespace Efm32
{
namespace ClockManagementUnit
{
	// Тип значения регистра
	typedef unsigned int ValueDataType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
	
	// CMU_CTRL - CMU Control Register
	template
		<
			RegisterAddressType Address
		>
	class ControlRegister
	{
	public:
		enum ClockOutput0Select
		{
			ClockOutput0SelectHFRCO			= 0,
			ClockOutput0SelectHFXO			= 1,
			ClockOutput0SelectHFCLK2		= 2,
			ClockOutput0SelectHFCLK4		= 3,
			ClockOutput0SelectHFCLK8		= 4,
			ClockOutput0SelectHFCLK16		= 5,
			ClockOutput0SelectULFRCO		= 6,
			ClockOutput0SelectAUXHFRCO		= 7,
		};
		
		enum ClockOutput1Select
		{
			ClockOutput1SelectLFRCO			= 0,
			ClockOutput1SelectLFXO			= 1,
			ClockOutput1SelectHFCLK			= 2,
			ClockOutput1SelectLFXOQ			= 3,
			ClockOutput1SelectHFXOQ			= 4,
			ClockOutput1SelectLFRCOQ		= 5,
			ClockOutput1SelectHFRCOQ		= 6,
			ClockOutput1SelectAUXHFRCOQ		= 7,
		};
		
		enum HFXOTimeout
		{
			HFXOTimeout8			= 0,
			HFXOTimeout256			= 1,
			HFXOTimeout1k			= 2,
			HFXOTimeout16k			= 3,
		};
	protected:
		struct RegStruct
		{
		ValueDataType HFXOMode						: 2;
		ValueDataType HFXOStartupBoostCurrent		: 2;
		ValueDataType 								: 1;
		ValueDataType HFXOBoostBufferCurrent		: 2;
		ValueDataType HFXOGlitchDetectorEnable		: 1;
		ValueDataType 								: 1;
		ValueDataType HFXOTimeout					: 2;
		ValueDataType LFXOMode						: 2;
		ValueDataType LFXOStartupBoostCurrent		: 1;
		ValueDataType HFCLKDivision					: 3;
		ValueDataType LFXOBoostBufferCurrent		: 1;
		ValueDataType LFXOTimeout					: 2;
		ValueDataType ClockOutputSelect0			: 3;
		ValueDataType ClockOutputSelect1			: 3;
		ValueDataType 								: 2;
		ValueDataType DebugClock					: 1;
		ValueDataType 								: 1;
		ValueDataType HighFrequencyLEInterface		: 1;
		ValueDataType 								: 1;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetClockOutput0Select(ClockOutput0Select value)
		{
			Register::Ptr()->ClockOutputSelect0 = value;
		}
		
		static void SetClockOutput1Select(ClockOutput1Select value)
		{
			Register::Ptr()->ClockOutputSelect1 = value;
		}
		
		static void SetHFXOTimeout(HFXOTimeout value)
		{
			Register::Ptr()->HFXOTimeout = value;
		}
	};
	
	// CMU_HFRCOCTRL - HFRCO Control Register
	template
		<
			RegisterAddressType Address
		>
	class HFRCOControlRegister
	{
	public:
		enum HFRCOBand
		{
			HFRCOBand1MHz		= 0,
			HFRCOBand7MHz		= 1,
			HFRCOBand11MHz		= 2,
			HFRCOBand14MHz		= 3,
			HFRCOBand21MHz		= 4,
			HFRCOBand28MHz		= 5,
		};
	protected:
		struct RegStruct
		{
		ValueDataType TuningValue		: 8;
		ValueDataType BandSelect		: 3;
		ValueDataType 					: 1;
		ValueDataType StartupDelay		: 5;
		ValueDataType 					: 15;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetHFRCOBand(HFRCOBand value)
		{
			Register::Ptr()->BandSelect = value;
		}
		
		static void SetHFRCOTuningValue(unsigned char value)
		{
			Register::Ptr()->TuningValue = value;
		}
	};
	
	template
		<
			RegisterAddressType Address
		>
	class HighFrequencyCoreClockEnableRegister
	{
	public:
	protected:
		struct RegStruct
		{
		ValueDataType DMA		: 1;
		ValueDataType AES		: 1;
		ValueDataType USBC		: 1;
		ValueDataType USB		: 1;
		ValueDataType LE		: 1;
		ValueDataType EBI		: 1;
		ValueDataType 			: 26;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockDma(bool enable = true)
		{
			Register::Ptr()->DMA = enable;
		}
		
		static void EnableClockAes(bool enable = true)
		{
			Register::Ptr()->AES = enable;
		}
		
		static void EnableClockUsbCore(bool enable = true)
		{
			Register::Ptr()->USBC = enable;
		}
		
		static void EnableClockUsb(bool enable = true)
		{
			Register::Ptr()->USB = enable;
		}
		
		static void EnableClockLe(bool enable = true)
		{
			Register::Ptr()->LE = enable;
		}
		
		static void EnableClockEbi(bool enable = true)
		{
			Register::Ptr()->EBI = enable;
		}
	};
	
	template
		<
			RegisterAddressType Address
		>
	class HighFrequencyPeripheralClockEnableRegister
	{
	public:
	protected:
		struct RegStruct
		{
		ValueDataType USART0	: 1;
		ValueDataType USART1	: 1;
		ValueDataType USART2	: 1;
		ValueDataType UART0		: 1;
		ValueDataType UART1		: 1;
		ValueDataType TIMER0	: 1;
		ValueDataType TIMER1	: 1;
		ValueDataType TIMER2	: 1;
		ValueDataType TIMER3	: 1;
		ValueDataType ACMP0		: 1;
		ValueDataType ACMP1		: 1;
		ValueDataType I2C0		: 1;
		ValueDataType I2C1		: 1;
		ValueDataType GPIO		: 1;
		ValueDataType VCMP		: 1;
		ValueDataType PRS		: 1;
		ValueDataType ADC0		: 1;
		ValueDataType DAC0		: 1;
		ValueDataType 			: 14;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockGpio(bool enable = true)
		{
			Register::Ptr()->GPIO = enable;
		}
		
		static void EnableClockUsart0(bool enable = true)
		{
			Register::Ptr()->USART0 = enable;
		}
		
		static void EnableClockUsart1(bool enable = true)
		{
			Register::Ptr()->USART1 = enable;
		}
		
		static void EnableClockUsart2(bool enable = true)
		{
			Register::Ptr()->USART2 = enable;
		}
		
		static void EnableClockUart0(bool enable = true)
		{
			Register::Ptr()->UART0 = enable;
		}
		
		static void EnableClockUart1(bool enable = true)
		{
			Register::Ptr()->UART1 = enable;
		}
		
		static void EnableClockTimer0(bool enable = true)
		{
			Register::Ptr()->TIMER0 = enable;
		}
		
		static void EnableClockTimer1(bool enable = true)
		{
			Register::Ptr()->TIMER1 = enable;
		}
		
		static void EnableClockTimer2(bool enable = true)
		{
			Register::Ptr()->TIMER2 = enable;
		}
		
		static void EnableClockTimer3(bool enable = true)
		{
			Register::Ptr()->TIMER3 = enable;
		}
		
		static void EnableClockAdc0(bool enable = true)
		{
			Register::Ptr()->ADC0 = enable;
		}
		
		static void EnableClockDac0(bool enable = true)
		{
			Register::Ptr()->DAC0 = enable;
		}

        static void EnableClockI2C0(bool enable = true)
        {
            Register::Ptr()->I2C0 = enable;
        }

        static void EnableClockI2C1(bool enable = true)
        {
            Register::Ptr()->I2C1 = enable;
        }
	};
	
	// CMU_OSCENCMD - Oscillator Enable/Disable Command Register
	template
		<
			RegisterAddressType Address
		>
	class OscillatorEnableDisableCommandRegister
	{
	public:
	protected:
		struct RegStruct
		{
		ValueDataType HFRCOEN		: 1;
		ValueDataType HFRCODIS		: 1;
		ValueDataType HFXOEN		: 1;
		ValueDataType HFXODIS		: 1;
		ValueDataType AUXHFRCOEN	: 1;
		ValueDataType AUXHFRCODIS	: 1;
		ValueDataType LFRCOEN		: 1;
		ValueDataType LFRCODIS		: 1;
		ValueDataType LFXOEN		: 1;
		ValueDataType LFXODIS		: 1;
		ValueDataType 				: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableHFRCO(bool enable = true)
		{
			if (enable)
			{
				Register::Ptr()->HFRCOEN = 1;
			}
			else
			{
				Register::Ptr()->HFRCODIS = 1;
			}
		}
		
		static void EnableHFXO(bool enable = true)
		{
			if (enable)
			{
				Register::Ptr()->HFXOEN = 1;
			}
			else
			{
				Register::Ptr()->HFXODIS = 1;
			}
		}
		
		static void EnableAUXHFRCO(bool enable = true)
		{
			if (enable)
			{
				Register::Ptr()->AUXHFRCOEN = 1;
			}
			else
			{
				Register::Ptr()->AUXHFRCODIS = 1;
			}
		}
		
		static void EnableLFRCO(bool enable = true)
		{
			if (enable)
			{
				Register::Ptr()->LFRCOEN = 1;
			}
			else
			{
				Register::Ptr()->LFRCODIS = 1;
			}
		}
		
		static void EnableLFXO(bool enable = true)
		{
			if (enable)
			{
				Register::Ptr()->LFXOEN = 1;
			}
			else
			{
				Register::Ptr()->LFXODIS = 1;
			}
		}
	};
	
	// CMU_CMD - Command Register
	template
		<
			RegisterAddressType Address
		>
	class CommandRegister
	{
	public:
		enum HightFrequencyClockSource
		{
			SetHightFrequencyClockSourceHFRCO = 1,
			SetHightFrequencyClockSourceHFXO = 2,
			SetHightFrequencyClockSourceLFRCO = 3,
			SetHightFrequencyClockSourceLFXO = 4
		};
	protected:
		struct RegStruct
		{
		ValueDataType HFCLKSEL		: 3;
		ValueDataType CALSTART		: 1;
		ValueDataType CALSTOP		: 1;
		ValueDataType USBCCLKSEL	: 1;
		ValueDataType 				: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetHightFrequencyClockSource(HightFrequencyClockSource val)
		{
			Register::Ptr()->HFCLKSEL = val;
		}
	};
	
	// CMU_STATUS - Status Register
	template
		<
			RegisterAddressType Address
		>
	class StatusRegister
	{
	public:
	protected:
		struct RegStruct
		{
		ValueDataType HFRCOENS		: 1;
		ValueDataType HFRCORDY		: 1;
		ValueDataType HFXOENS		: 1;
		ValueDataType HFXORDY		: 1;
		ValueDataType AUXHFRCOENS	: 1;
		ValueDataType AUXHFRCORDY	: 1;
		ValueDataType LFRCOENS		: 1;
		ValueDataType LFRCORDY		: 1;
		ValueDataType LFXOENS		: 1;
		ValueDataType LFXORDY		: 1;
		ValueDataType HFRCOSEL		: 1;
		ValueDataType HFXOSEL		: 1;
		ValueDataType LFRCOSEL		: 1;
		ValueDataType LFXOSEL		: 1;
		ValueDataType CALBSY		: 1;
		ValueDataType USBCHFCLKSEL	: 1;
		ValueDataType USBCLFXOSEL	: 1;
		ValueDataType USBCLFRCOSEL	: 1;
		ValueDataType 				: 22;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool ReadyHFRCO()
		{
			return Register::Ptr()->HFRCORDY;
		}
		
		static bool ReadyHFXO()
		{
			return Register::Ptr()->HFXORDY;
		}
		
		static bool ReadyAUXHFRCO()
		{
			return Register::Ptr()->AUXHFRCORDY;
		}
		
		static bool ReadyLFRCO()
		{
			return Register::Ptr()->LFRCORDY;
		}
		
		static bool ReadyLFXO()
		{
			return Register::Ptr()->LFXORDY;
		}
		
		static bool HFXOSelected()
		{
			return Register::Ptr()->HFXOSEL;
		}
	};
	
	// CMU_LFACLKEN0 - Low Frequency A Clock Enable Register 0(Async Reg)
	template
		<
			RegisterAddressType Address
		>
	class LowFrequencyAClockEnableRegister
	{
	public:
	protected:
		struct RegStruct
		{
			ValueDataType LESENSE	: 1;
			ValueDataType RTC		: 1;
			ValueDataType LETIMER0	: 1;
			ValueDataType LCD		: 1;
			ValueDataType 			: 28;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockLeSensorInterface(bool enable = true)
		{
			Register::Ptr()->LESENSE = enable;
		}
		
		static void EnableClockRtc(bool enable = true)
		{
			Register::Ptr()->RTC = enable;
		}
		
		static void EnableClockLeTimer0(bool enable = true)
		{
			Register::Ptr()->LETIMER0 = enable;
		}
		
		static void EnableClockLcd(bool enable = true)
		{
			Register::Ptr()->LCD = enable;
		}
	};
	
	// CMU_LFBCLKEN0 - Low Frequency B Clock Enable Register 0(Async Reg)
	template
		<
			RegisterAddressType Address
		>
	class LowFrequencyBClockEnableRegister
	{
	public:
	protected:
		struct RegStruct
		{
			ValueDataType LEUART0	: 1;
			ValueDataType LEUART1	: 1;
			ValueDataType 			: 30;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void EnableClockLeUart0(bool enable = true)
		{
			Register::Ptr()->LEUART0 = enable;
		}
		
		static void EnableClockLeUart1(bool enable = true)
		{
			Register::Ptr()->LEUART1 = enable;
		}
	};
	
	// CMU_ROUTE - I/O Routing Register
	template
		<
			RegisterAddressType Address
		>
	class IORouteRegister
	{
	public:
		enum Location
		{
			Location0 = 0,
			Location1 = 1,
			Location2 = 2,
		};
	protected:
		struct RegStruct
		{
		ValueDataType CLKOUT0PinEnable		: 1;
		ValueDataType CLKOUT1PinEnable		: 1;
		ValueDataType IOLocation			: 3;
		ValueDataType 						: 26;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetPinEnable(bool pin0, bool pin1)
		{
			Register::Ptr()->CLKOUT0PinEnable = pin0;
			Register::Ptr()->CLKOUT1PinEnable = pin1;
		}
		
		static void SetIOLocation(Location val)
		{
			Register::Ptr()->IOLocation = val;
		}
	};
	
	template
		<
			RegisterAddressType BaseAddr,
			RegisterAddressType ControlRegAddr,
			RegisterAddressType HighFrequencyCoreClockDivisionRegAddr,
			RegisterAddressType HighFrequencyPeripheralClockDivisionRegAddr,
			RegisterAddressType HFRCOControlRegAddr,
			RegisterAddressType LFRCOControlRegAddr,
			RegisterAddressType AUXHFRCOControlRegAddr,
			RegisterAddressType CalibrationControlRegAddr,
			RegisterAddressType CalibrationCounterRegAddr,
			RegisterAddressType OscillatorEnableDisableCommandRegAddr,
			RegisterAddressType CommandRegAddr,
			RegisterAddressType LowFrequencyClockSelectRegAddr,
			RegisterAddressType StatusRegAddr,
			RegisterAddressType InterruptFlagRegAddr,
			RegisterAddressType InterruptFlagSetRegAddr,
			RegisterAddressType InterruptFlagClearRegAddr,
			RegisterAddressType InterruptEnableRegAddr,
			RegisterAddressType HighFrequencyCoreClockEnableRegAddr,
			RegisterAddressType HighFrequencyPeripheralClockEnableRegAddr,
			RegisterAddressType SynchronizationBusyRegAddr,
			RegisterAddressType FreezeRegAddr,
			RegisterAddressType LowFrequencyAClockEnableRegAddr,
			RegisterAddressType LowFrequencyBClockEnableRegAddr,
			RegisterAddressType LowFrequencyAPrescalerRegAddr,
			RegisterAddressType LowFrequencyBPrescalerRegAddr,
			RegisterAddressType PCNTControlRegAddr,
			RegisterAddressType LCDControlRegAddr,
			RegisterAddressType IORoutingRegAddr,
			RegisterAddressType ConfigurationLockRegAddr
		>
	class ClockManagementUnit :
		public ControlRegister<BaseAddr + ControlRegAddr>,
		public HFRCOControlRegister<BaseAddr + HFRCOControlRegAddr>,
		public HighFrequencyCoreClockEnableRegister<BaseAddr + HighFrequencyCoreClockEnableRegAddr>,
		public HighFrequencyPeripheralClockEnableRegister<BaseAddr + HighFrequencyPeripheralClockEnableRegAddr>,
		public OscillatorEnableDisableCommandRegister<BaseAddr + OscillatorEnableDisableCommandRegAddr>,
		public CommandRegister<BaseAddr + CommandRegAddr>,
		public StatusRegister<BaseAddr + StatusRegAddr>,
		public LowFrequencyAClockEnableRegister<BaseAddr + LowFrequencyAClockEnableRegAddr>,
		public LowFrequencyBClockEnableRegister<BaseAddr + LowFrequencyBClockEnableRegAddr>,
		public IORouteRegister<BaseAddr + IORoutingRegAddr>
	{
	};
}
}
}

#endif