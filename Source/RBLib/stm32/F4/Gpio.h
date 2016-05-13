///////////////////////////////////////////////////////////////////////////////
//	Класс управления портами ввода-вывода
//	STM32
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GPIO_STM32_F4_H
#define GPIO_STM32_F4_H

#include "Register.h"
#include "../../IdObjectBase.h"
#include "../../Utils/CallbackListWrapper.h"

namespace Rblib
{
namespace Stm32
{
namespace F4
{
namespace GpioImplementation
{
	// GPIO port mode register (GPIOx_MODER) (x = A..I/J/K)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortModeRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x00;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum Mode
		{
			ModeInput			= 0,
			ModeOutput			= 1,
			ModeAlternate		= 2,
			ModeAnalog			= 3,
		};
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void SetMode(unsigned char pin, Mode val)
		{
			volatile Core::RegisterValueType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 15)
			{
				ptr = Register::Ptr();
				offset = pin * 2;
			}
			
			if (ptr)
			{
				Core::RegisterValueType regVal = *ptr;
				regVal &= ~(0x03 << offset);
				regVal |= val << offset;
				
				*ptr = regVal;
			}
		}
		
		static void SetOutputPin(unsigned char pin)
		{
			SetMode(pin, ModeOutput);
		}
	};
	
	// GPIO port output type register (GPIOx_OTYPER) (x = A..I/J/K)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortOutputTypeRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x4;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum OutputType
		{
			OutputPushPull	= 0,
			OutputOpenDrain	= 1,
		};
		
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void SetOuputType(unsigned char pin, OutputType val)
		{
			*Register::Ptr() &= ~(1 << pin);
			*Register::Ptr() |= val << pin;
		}
	};
	
	// GPIO port output speed register (GPIOx_OSPEEDR) (x = A..I/J/K)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortOutputSpeedRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x08;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum Speed
		{
			SpeedLow			= 0,
			SpeedMedium			= 1,
			SpeedHight			= 2,
			SpeedVeryHight		= 3,
		};
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void SetOutputSpeed(unsigned char pin, Speed val)
		{
			volatile Core::RegisterValueType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 15)
			{
				ptr = Register::Ptr();
				offset = pin * 2;
			}
			
			if (ptr)
			{
				Core::RegisterValueType regVal = *ptr;
				regVal &= ~(0x03 << offset);
				regVal |= val << offset;
				
				*ptr = regVal;
			}
		}
	};
	
	// GPIO port pull-up/pull-down register (GPIOx_PUPDR) (x = A..I/J/K)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortPullUpPullDownRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x0C;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
		
		enum PullUpPullDownMode
		{
			PullUpPullDownModeNone 			= 0,
			PullUpPullDownModePullUp		= 1,
			PullUpPullDownModePullDown		= 2,
		};
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void SetPullUpPullDownMode(unsigned char pin, PullUpPullDownMode val)
		{
			volatile Core::RegisterValueType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 15)
			{
				ptr = Register::Ptr();
				offset = pin * 2;
			}
			
			if (ptr)
			{
				Core::RegisterValueType regVal = *ptr;
				regVal &= ~(0x03 << offset);
				regVal |= val << offset;
				
				*ptr = regVal;
			}
		}
	};
	
	//GPIO port input data register (GPIOx_IDR) (x = A..I/J/K)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortInputDataRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x10;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static Core::RegisterValueType Read()
		{
			return *Register::Ptr();
		}
		
		static Core::RegisterValueType Read(unsigned char pin)
		{
			return *Register::Ptr() & (1 << pin);
		}
	};
	
	// GPIO port output data register (GPIOx_ODR) (x = A..I/J/K)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortOutputDataRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x14;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void Write(Core::RegisterValueType val)
		{
			*Register::Ptr() = val;
		}
	};
	
	// GPIO port bit set/reset register (GPIOx_BSRR) (x = A..I/J/K)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortBitSetResetRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffset = 0x18;
		static const Core::RegisterAddressType Address = BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, Core::RegisterValueType> Register;
	public:
		static void SetBit(unsigned char pin)
		{
			*Register::Ptr() = 1 << pin;
		}
		
		static void ClearBit(unsigned char pin)
		{
			*Register::Ptr() = 1 << (pin + 16);
		}
	};
	
	// GPIO alternate function low register (GPIOx_AFRL) (x = A..I/J/K)
	// GPIO alternate function high register (GPIOx_AFRH) (x = A..I/J)
	template
		<
			Core::RegisterAddressType BaseAddress
		>
	class PortAlternateFunctionRegister
	{
	public:
		static const Core::RegisterAddressType AddressOffsetLow = 0x20;
		static const Core::RegisterAddressType AddressOffsetHi = 0x24;
		static const Core::RegisterAddressType AddressLow = BaseAddress + AddressOffsetLow;
		static const Core::RegisterAddressType AddressHi = BaseAddress + AddressOffsetHi;
	protected:
		typedef Register<AddressLow, Core::RegisterValueType> RegisterLow;
		typedef Register<AddressHi, Core::RegisterValueType> RegisterHi;
	public:
		static void SetAlternateFunctionNumber(unsigned char pin, unsigned char functionNumber)
		{
			volatile Core::RegisterValueType *ptr = 0;
			unsigned char offset = 0;
			
			if (pin <= 7)
			{
				ptr = RegisterLow::Ptr();
				offset = pin * 4;
			}
			
			if (pin >= 8 && pin <= 15)
			{
				ptr = RegisterHi::Ptr();
				offset = (pin - 8) * 4;
			}
			
			if (ptr)
			{
				Core::RegisterValueType regVal = *ptr;
				regVal &= ~(0x03 << offset);
				regVal |= functionNumber << offset;
				
				*ptr = regVal;
			}
		}
	};
	
	template
		<
			unsigned int IdObj,
			Core::RegisterAddressType BaseAddress
		>
	class Port :
		public IdObjectBase<IdObj>,
		public PortModeRegister<BaseAddress>,
		public PortOutputTypeRegister<BaseAddress>,
		public PortOutputSpeedRegister<BaseAddress>,
		public PortPullUpPullDownRegister<BaseAddress>,
		public PortInputDataRegister<BaseAddress>,
		public PortOutputDataRegister<BaseAddress>,
		public PortBitSetResetRegister<BaseAddress>,
		public PortAlternateFunctionRegister<BaseAddress>
	{
	};
	
	class Gpio
	{
	public:
		typedef Port<1, 0x40020000> A;
		typedef Port<2, 0x40020400> B;
		typedef Port<3, 0x40020800> C;
		typedef Port<4, 0x40020C00> D;
		typedef Port<5, 0x40021000> E;
		typedef Port<6, 0x40021400> F;
		typedef Port<7, 0x40021800> G;
		typedef Port<1, 0x40021C00> H;
		typedef Port<1, 0x40022000> I;
		typedef Port<1, 0x40022400> J;
		typedef Port<1, 0x40022800> K;
	};
}

typedef GpioImplementation::Gpio Gpio;
}
}
}

#endif